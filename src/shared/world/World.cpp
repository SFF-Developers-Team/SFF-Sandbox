#include "Types.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <entity/Player.hpp>
#include <world/World.hpp>
#include <Logger.hpp>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <Logger.hpp>
#include <world/Structure.hpp>
#include <FastNoiseLite.h>
#include <random>
#include <numbers>

World::World() {
    m_tiles.fill(TileID::AIR);
}

float World::HashNoise(int seed, int x, int y) {
    uint32_t h = static_cast<uint32_t>(seed);

    h ^= static_cast<uint32_t>(x) * 0x9E3779B1u;
    h ^= static_cast<uint32_t>(y) * 0x85EBCA77u;

    h ^= h >> 16;
    h *= 0x7FEB352Du;
    h ^= h >> 15;
    h *= 0x846CA68Bu;
    h ^= h >> 16;

    return float(h >> 8) * (1.f / 16777216.f);
}

std::unique_ptr<World> World::GenerateWorld(int seed) {
    auto ret = std::make_unique<World>();

    std::vector<int> surfaceHeights(WORLD_SIZE);
    std::vector<TileID> surfaceBlocks(WORLD_SIZE);

    FastNoiseLite mountains;
    FastNoiseLite hills;
    FastNoiseLite details;
    FastNoiseLite climate;
    FastNoiseLite biomeEdges;
    FastNoiseLite materialNoise;

    mountains.SetSeed(seed);
    hills.SetSeed(seed + 1);
    details.SetSeed(seed + 2);
    climate.SetSeed(seed + 3);
    biomeEdges.SetSeed(seed + 4);
    materialNoise.SetSeed(seed + 5);

    mountains.SetFrequency(0.002f);
    hills.SetFrequency(0.012f);
    details.SetFrequency(0.06f);
    climate.SetFrequency(0.005f);
    biomeEdges.SetFrequency(0.04f);
    materialNoise.SetFrequency(0.2f);

    const int baseHeight = 216;

    auto smoothstep = [](float a, float b, float x) {
        float t = std::clamp((x - a) / (b - a), 0.f, 1.f);
        return t * t * (3.f - 2.f * t);
    };

    auto spray = [&](int x, int y) {
        float grain = World::HashNoise(seed, x, y);

        float cloud = 0.5f + 0.5f * materialNoise.GetNoise(float(x), float(y));

        return grain * 0.75f + cloud * 0.25f;
    };

    // Mountains / Hills / Small deformations
    constexpr std::array<float, 3> forestN = {10.f, 5.f, 1.f};
    constexpr std::array<float, 3> snowN = {50.f, 9.f, 2.f};
    constexpr std::array<float, 3> desertN = {0.f, 1.f, 1.f};

    // Generating surface

    for (int x = 0; x < WORLD_SIZE; ++x) {
        float temperature = climate.GetNoise(float(x), 0.f);
        float snowWeight = 1.f - smoothstep(-0.05f, 0.25f, temperature);
        float desertWeight = smoothstep(0.35f, 0.65f, temperature);

        std::array<float, 3> N;

        for (int i = 0; i < 3; i++) {
            N[i] = forestN[i] + (snowN[i] - forestN[i]) * snowWeight + (desertN[i] - forestN[i]) * desertWeight;
        }

        float edgeNoise = biomeEdges.GetNoise(float(x), 0.f) * 0.25f;

        TileID surfaceBlock = TileID::GRASS;
        TileID depthBlock = TileID::DIRT;

        if (snowWeight > 0.5f + edgeNoise) {
            surfaceBlock = TileID::SNOW;
        } else if (desertWeight > 0.5f + edgeNoise) {
            surfaceBlock = TileID::SAND;
            depthBlock = TileID::SAND;
        }

        float height = baseHeight
            + mountains.GetNoise(float(x), 0.f) * N[0]
            + hills.GetNoise(float(x), 0.f) * N[1]
            + details.GetNoise(float(x), 0.f) * N[2];

        int surfaceY = std::clamp(static_cast<int>(height), 0, WORLD_SIZE - 1);

        {
            float sample = spray(x, surfaceY);

            bool snow = sample < snowWeight;
            bool sand = sample < desertWeight;

            if (snow) 
                surfaceBlock = TileID::SNOW;
            else if (sand)
                surfaceBlock = TileID::SAND;
            else
                surfaceBlock = TileID::GRASS;
        }

        ret->setTile(TilePosition{x, surfaceY, 1}, surfaceBlock);
        ret->setTile(TilePosition{x, surfaceY, 0}, surfaceBlock);

        int stoneStart = surfaceY + 30 + static_cast<int>(hills.GetNoise(float(x), 123.0f) * 8.f);

        // Under surface
        for (int y = surfaceY + 1; y < WORLD_SIZE; ++y) {
            TileID block;

            if (y == WORLD_SIZE - 1) {
                block = TileID::BEDROCK;
            } else if (y < stoneStart) {
                float sample = spray(x, y);
                block = sample < desertWeight ? TileID::SAND : TileID::DIRT;
            } else {
                block = TileID::STONE;
            }

            ret->setTile(TilePosition{x, y, 1}, block);
            ret->setTile(TilePosition{x, y, 0}, block);
        }
        
        surfaceHeights[x] = surfaceY;
        surfaceBlocks[x] = surfaceBlock;
    }

    // Planting

    int lastCactusX = -100;

    for (int terrainX = 0; terrainX < WORLD_SIZE; ++terrainX) {
        int surfaceY = surfaceHeights[terrainX];
        int plantY = surfaceY - 1;

        if (plantY < 0)
            continue;

        TileID ground = surfaceBlocks[terrainX];

        float chance = HashNoise(seed, terrainX, -1000);

        switch (ground) {
            case TileID::GRASS: {
                TileID plant = TileID::AIR;

                if (chance < 0.80f) {
                    float type = HashNoise(seed, terrainX, -2000);

                    if (type < 0.20f)
                        plant = TileID::TALL_GRASS;
                    else if (type < 0.60f)
                        plant = TileID::MEDIUM_GRASS;
                    else
                        plant = TileID::TINY_GRASS;
                } else if (chance < 0.83f) {
                    plant = TileID::ROSE;
                } else if (chance < 0.90f) {
                    for (int x = -2; x <= 2; x++) {
                        for (int y = -2; y <= 2; y++) {
                            if (x * x + y * y <= 5) {
                                ret->setTile(TilePosition{terrainX + x, plantY - 5 + y, 0}, TileID::LEAVES);
                                ret->setTile(TilePosition{terrainX + x, plantY - 5 + y, 1}, TileID::LEAVES);
                            }
                        }
                    }

                    for (int y = 0; y < 6; y++) {
                        ret->setTile(TilePosition{terrainX, plantY - y, 0}, TileID::OAK_LOG);
                    }
                    
                    break;
                }

                if (plant != TileID::AIR) {
                    ret->setTile(
                        TilePosition{terrainX, plantY, 1},
                        plant
                    );
                }

                break;
            }

            case TileID::SNOW: {
                ret->setTile(TilePosition{terrainX, plantY, 1}, TileID::SNOW_CARPET);
                break;
            }

            case TileID::SAND: {
                if (terrainX - lastCactusX < 4)
                    break;

                if (chance >= 0.08f)
                    break;

                for (int i = 0; i < 2; ++i) {
                    TileID part = (i == 1) ? TileID::CACTUS_HEAD : TileID::CACTUS_BASE;

                    ret->setTile(TilePosition{terrainX, plantY - i, 1}, part);
                }

                lastCactusX = terrainX;
                break;
            }

            default:
                break;
        }
    }

    // Caving

    auto cave = GenerateCave(seed, 6);

    for (int i = 0; i < WORLD_SIZE*WORLD_SIZE; i++) {
        if (!cave[i] && ret->m_tiles[WORLD_SIZE*WORLD_SIZE + i] == TileID::STONE) 
            ret->m_tiles[WORLD_SIZE*WORLD_SIZE + i] = TileID::AIR; 
    }

    float angle = 1.69f;
    float turnStrength = 0.1f;
    float radiusVariation = 0.9f;

    FastNoiseLite turnNoise(seed + 12);
    FastNoiseLite radiusNoise(seed + 13);
    // turnNoise.SetFrequency(0.010f);
    // radiusNoise.SetFrequency(0.010f);

    const int speed = 3;
    const int baseRadius = 3;

    std::mt19937 caveRng(seed + 14);
    std::uniform_real_distribution<float> chance(0.f, 1.f);
    std::uniform_real_distribution<float> direction(0.f, 2.f * std::numbers::pi_v<float>);

    int caveIndex = 0;

    for (int regionY = 0; regionY < WORLD_SIZE; regionY += 120) {
        for (int regionX = 0; regionX < WORLD_SIZE; regionX += 150) {
            if (chance(caveRng) >= 0.6f) {
                continue;
            }

            int maxX = std::min(regionX + 149, WORLD_SIZE - 1);
            int startX = std::uniform_int_distribution<int>(regionX, maxX)(caveRng);

            int minY = std::min(regionY, surfaceHeights[startX] + 25);
            int maxY = std::min(regionY + 119, WORLD_SIZE - 1);

            if (minY > maxY) {
                continue;
            }

            int startY = std::uniform_int_distribution<int>(minY, maxY)(caveRng);
            Vec2f wormPosition{static_cast<float>(startX), static_cast<float>(startY)};
            float angle = direction(caveRng);
            int length = std::uniform_int_distribution<int>(50, 170)(caveRng);

            for (int step = 0; step < length; step++) {
                angle += turnNoise.GetNoise(float(step), float(caveIndex) * 100.f) * turnStrength;

                wormPosition.x += std::cos(angle) * speed;
                wormPosition.y += std::sin(angle) * speed;

                Vec2i tilePosition = wormPosition.as<int>();

                if (tilePosition.y < surfaceHeights[tilePosition.x] + 5) {
                    break;
                }

                float radius = baseRadius + radiusNoise.GetNoise(float(step), float(caveIndex) * 100.f) * radiusVariation;
                radius = std::max(1.f, radius);

                CarveCircle(*ret, {tilePosition.x, tilePosition.y, 1}, radius);
            }

            caveIndex++;
        }
    }

    return ret;
}

void World::CarveCircle(World& world, TilePosition position, int radius) {
    Vec2i pos{position.x, position.y};
    for (int y = position.y - radius; y < position.y + radius; y++) {
        for (int x = position.x - radius; x < position.x + radius; x++) {
            if (pos.distance(Vec2i{x, y}) <= radius) {
                world.setTile(TilePosition{x, y, position.layer}, TileID::AIR);
            }
        }
    }
}

World::CaveMap World::GenerateCave(int seed, int iterations) {
    std::mt19937 rng(seed);
    std::bernoulli_distribution wallChance(0.63);

    CaveMap map(WORLD_SIZE * WORLD_SIZE);
    CaveMap next(WORLD_SIZE * WORLD_SIZE);

    auto index = [](int x, int y) { return y * WORLD_SIZE + x; };

    for (int y = 0; y < WORLD_SIZE; ++y) {
        for (int x = 0; x < WORLD_SIZE; ++x) {
            bool border = x == 0 || y == 0 || x == WORLD_SIZE - 1 || y == WORLD_SIZE - 1;

            map[index(x, y)] = border || wallChance(rng);
        }
    }

    for (int i = 0; i < iterations; ++i) {
        for (int y = 0; y < WORLD_SIZE; ++y) {
            for (int x = 0; x < WORLD_SIZE; ++x) {
                int walls = 0;

                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0)
                            continue;

                        int nx = x + dx;
                        int ny = y + dy;

                        if (nx < 0 || ny < 0 ||
                            nx >= WORLD_SIZE || ny >= WORLD_SIZE) {
                            ++walls;
                        } else {
                            walls += map[index(nx, ny)];
                        }
                    }
                }

                bool border = x == 0 || y == 0 || x == WORLD_SIZE - 1 || y == WORLD_SIZE - 1;
                next[index(x, y)] = border || walls >= 5;
            }
        }

        map.swap(next);
    }

    return map;
}

void World::onTick() {
    m_time++;

    for (auto& [id, player] : m_players) {
        player->onTick(*this);

        auto position = player->getPosition();

        int distance = 20;

        for (auto layer = 0; layer < WORLD_LAYERS; layer++) {
            for (int x = position.x - distance; x < position.x + distance; x++) {
                for (int y = position.y - distance; y < position.y + distance; y++) {
                    auto tile0 = Tile::Get(getTile({x, y, 0}));
                    if (tile0 != nullptr) {
                        tile0->onTick(*this, {x, y, 0});
                    }

                    auto tile1 = Tile::Get(getTile({x, y, 1}));
                    if (tile1 != nullptr) {
                        tile1->onTick(*this, {x, y, 1});
                    }
                }
            }
        }

        for (int i = 0; i < m_randomTickSpeed; i++) {
            std::uniform_int_distribution<int> positionX(position.x - distance, position.x + distance);
            std::uniform_int_distribution<int> positionY(position.y - distance, position.y + distance);

            int x = positionX(m_rng);
            int y = positionY(m_rng);

            auto tile0 = Tile::Get(getTile({x, y, 0}));
            if (tile0 != nullptr) {
                tile0->onRandomTick(*this, {x, y, 0});
            }

            auto tile1 = Tile::Get(getTile({x, y, 1}));
            if (tile1 != nullptr) {
                tile1->onRandomTick(*this, {x, y, 1});
            }
        }
    }

    for (auto& [playerId, info] : m_breakInfo) {
        auto player = m_players[playerId];
        auto tile = getTile(info.pos);
        float speed = Tile::GetBreakingSpeed(tile, player->getItemHand().item.get());
        float d = Tile::GetDurability(tile);
    
        info.progress += speed / d * 0.01f;
    
        if (info.progress >= d || player->getGamemode() == GAMEMODE_CREATIVE) {
            auto drop = Tile::GetDrop(tile, player->getItemHand().item.get());

            if (player->getGamemode() != GAMEMODE_CREATIVE && drop.has_value()) {
                player->addItem(std::make_unique<Item>(*drop));
            }

            destroyTile(info.pos);

            auto destroyedTile = Tile::Get(tile);

            if (destroyedTile != nullptr) {
                destroyedTile->tileChanged(*this, info.pos);
            }
            
            player->setBreakingTile(false);
        }
    }

    std::erase_if(m_breakInfo, [this](auto& info) {
        auto tile = getTile(info.second.pos);
        return tile == TileID::AIR;
    });
}

bool World::isTileClosed(TilePosition pos) const {
    if (TileID::AIR != getTile({pos.x - 1, pos.y, pos.layer}) || 
        TileID::AIR != getTile({pos.x + 1, pos.y, pos.layer}) || 
        TileID::AIR != getTile({pos.x, pos.y - 1, pos.layer}) || 
        TileID::AIR != getTile({pos.x, pos.y + 1, pos.layer}) || 
        TileID::AIR != getTile({pos.x, pos.y, static_cast<uint8_t>(pos.layer ^ 1)})) {
        return false;
    }

    return true;
}

bool World::save() const {
    // if (!std::filesystem::exists(m_saveDir) && !std::filesystem::create_directory(m_saveDir)) {
    //     logE("Failed to save world!");
    //     return false;
    // }

    // // for (auto [pos, _] : m_chunks) {
    // //     saveChunk(pos);
    // // }

    // for (auto [id, _] : m_players) {
    //     savePlayer(id);
    // }

    // auto worldData = DataStream();
    // worldData.add(WORLD_VERSION);
    // // worldData.add(m_worldGen->getType());
    // // worldData.add(m_worldGen->getSeed());
    // worldData.add(m_time);

    // auto worldFile = std::ofstream(m_saveDir / "world.dat", std::ios::binary);
    // if(!worldFile.is_open()) {
    //     logE("Failed to save world data!");
    //     return false;
    // }
    
    // worldFile.write((char const*)worldData.data(), worldData.size());

    return false;
}

bool World::load() {
    // if (!std::filesystem::exists(m_saveDir)) {
    //     logE("World directory not found!");
    //     return false;
    // }

    // if (!std::filesystem::exists(m_saveDir / "world.dat")) {
    //     logE("world.dat not found!");
    //     return false;
    // }

    // std::ifstream file(m_saveDir / "world.dat", std::ios::in | std::ios::binary);

    // if (!file.is_open()) {
    //     logE("Can't open world.dat");
    //     return false;
    // }

    // file.seekg(0, std::ios::end);
    // int worldSize = file.tellg();

    // file.seekg(0, std::ios::beg);
    // auto worldData = DataStream(worldSize);

    // file.read((char*)worldData.data(), worldData.size());

    // m_version = worldData.get<uint32_t>();

    // if (m_version == 256 && worldData.getI<uint32_t>() == 128) {
    //     logE("Old world version detected! We don't support it now...");
    //     return false;
    // }

    // // We have only one world gen type now...
    // auto genType = worldData.get<WorldGenType>();
    // auto seed = worldData.get<uint64_t>();

    // // m_worldGen = std::make_shared<WorldGenNormal>(seed);
    // m_time = worldData.get<uint64_t>();

    // for (const auto& entry : std::filesystem::directory_iterator(m_saveDir / "chunks")) {
    //     std::stringstream ss(entry.path().stem().string());
    //     std::string part;
    //     Vec2i chunkPosition;
    
    //     std::getline(ss, part, ',');
    //     chunkPosition.x = std::stoi(part);
    //     std::getline(ss, part, ',');
    //     chunkPosition.y = std::stoi(part);
    
    
    //     loadChunk(chunkPosition);
    // }


    return false;
}

std::vector<Hitbox> World::getHitboxes(Hitbox entityHitbox, int radius) {
    int minX = entityHitbox.x - radius;
    int maxX = ceil(entityHitbox.x + entityHitbox.width * radius);
    int minY = entityHitbox.y - radius;
    int maxY = ceil(entityHitbox.y + entityHitbox.height * radius);

    std::vector<Hitbox> ret;

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            auto hitbox = getTileHitbox({x, y, 1});

            if (hitbox.has_value()) {
                ret.push_back(hitbox.value());
            }
        }
    }

    return ret;
}

void World::setPlayer(PlayerID id, std::shared_ptr<Player> player, std::string const username) {
    m_players[id] = player;
    player->setUsername(username);
}

PlayerID World::addPlayer(std::shared_ptr<Player> player, std::string const username) {
    auto it = std::find_if(m_players.begin(), m_players.end(), [username](auto& pair) { 
        return pair.second->getUsername() == username;
    });

    if (it != m_players.end()) {
        return 0;
    }

    m_players[m_lastPlayerID] = player;
    player->setID(m_lastPlayerID);
    player->setUsername(username);

    logD("Added player with id {}", player->getPlayerID());

    return m_lastPlayerID++;
}

std::shared_ptr<Player> World::getPlayer(PlayerID id) const {
    auto it = m_players.find(id);
    return (it != m_players.end()) ? it->second : nullptr;
}

void World::unloadPlayer(PlayerID id) {
    savePlayer(id);
    m_players.erase(id);
}

bool World::savePlayer(PlayerID id) const {
    // auto player = m_players[id];
    // auto pbytes = player->serialize();

    // if (!pbytes.size()) {
    //     logE("Failed to serialize player {}!", id);
    // }

    // auto username = player->getUsername();

    // std::ofstream file(m_saveDir / "players" / ((username.empty() ? "player" : username) + ".dat"), std::ios::binary);
    
    // if(file.is_open()) {
    //     file.write((char const*)pbytes.data(), pbytes.size());
    //     file.close();

    //     return true;
    // }

    // logE("Failed to save player {}!", id);
    return false;
}

bool World::loadPlayer(std::string const& username) {
    // auto it = std::find_if(m_players.begin(), m_players.end(), [username](auto&& pair) {
    //     return pair.second->getUsername() == username;
    // });

    // if (it == m_players.end()) {
    //     it->second = std::make_shared<SimplePlayer>(std::shared_ptr<World>(this));
    // }

    // std::ifstream file(m_saveDir / "players" / ((username.empty() ? "player" : username) + ".dat"), std::ios::in | std::ios::binary);
        
    // if (!file.is_open()) {
    //     return false;
    // }

    // file.seekg(0, std::ios::end);
    // int worldSize = file.tellg();

    // file.seekg(0, std::ios::beg);
    // auto playerBytes = DataStream(worldSize);

    // file.read((char*)playerBytes.data(), playerBytes.size());
    // file.close();

    // it->second->deserialize(playerBytes);
    // it->second->setUsername(username);

    return false;
}

bool World::isUsernameAlreadyTaken(std::string const& username) const {
    auto it = std::find_if(m_players.begin(), m_players.end(), [username](auto&& pair) {
        return pair.second->getUsername() == username;
    });

    return it != m_players.end();
}

std::optional<Rectf> World::getTileHitbox(TilePosition pos) {
    if (pos.layer > 0) {
        auto tile = getTile(pos);

        if (tile != TileID::AIR && !Tile::IsPhantom(tile)) {
            return Rectf {static_cast<float>(pos.x), static_cast<float>(pos.y), 1.f, 1.f};
        } 
    }

    return std::nullopt;
}

bool World::isOutOfBound(TilePosition pos) const {
    return pos.x < 0 || pos.x >= WORLD_SIZE || pos.y < 0 || pos.y >= WORLD_SIZE || pos.layer < 0 || pos.layer >= WORLD_LAYERS;
}

void World::placeTile(PlayerID m_id, TilePosition pos) {
    auto player = m_players[m_id];
    auto& slot = player->getItemHand();

    if (slot.item != nullptr && Item::IsTileItem(slot.item->getID()) && getTile(pos) == TileID::AIR) {
        auto newTile = Item::GetTile(slot.item->getID());
        setTile(pos, newTile);

        auto placedTile = Tile::Get(newTile);

        if (placedTile != nullptr) {
            placedTile->tileChanged(*this, pos);
        }

        if(player->getGamemode() == GAMEMODE_SURVIVAL) {
            slot.count--;
            player->gc();
        }
    }
}

void World::breakTile(PlayerID id, TilePosition pos) {
    // destroyTile(pos);
    auto player = m_players[id];
    auto tileId = getTile(pos);

    // Bedrock has -1.f
    if (Tile::GetDurability(tileId) < 0) {
        return;
    }

    auto it = m_breakInfo.find(id);

    if (it != m_breakInfo.end()) {
        if(it->second.pos != pos) {
            it->second.pos = pos;
            it->second.progress = 0;
            it->second.time = m_time;
        }
        
        return;
    }

    m_breakInfo.insert(std::make_pair(id, BreakInfo {pos, m_time, 0}));
    player->setBreakingTile(true);
}

void World::stopBreakingTile(PlayerID id) {
    m_players[id]->setBreakingTile(false);
    m_breakInfo.erase(id);   
}

void World::tileChanged(TilePosition position) {
    auto newTile = getTile(position);
    auto tile = Tile::Get(newTile);

    if (tile != nullptr) {
        tile->tileChanged(*this, position);
    }
}