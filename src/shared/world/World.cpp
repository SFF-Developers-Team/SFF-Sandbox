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
#include <Logger.hpp>
#include <world/Structure.hpp>
#include <FastNoiseLite.h>

#define RENDER_DISTANCE 9

World::World(std::filesystem::path const& saveDir) : m_saveDir(saveDir), m_version(World::WORLD_VERSION), m_time(0), m_lastPlayerID(1), m_randomTickSpeed(3) {
    if (!saveDir.empty() && !std::filesystem::exists(m_saveDir)) {
        std::filesystem::create_directory(m_saveDir);
        std::filesystem::create_directory(m_saveDir / "chunks");
        std::filesystem::create_directory(m_saveDir / "players");
    }
}

void World::generate() {
    // TODO: Move to worldgen
    m_blocks.fill(BlockID::AIR);

    FastNoiseLite mountains;
    FastNoiseLite hills;
    FastNoiseLite details;

    int seed = 0;

    mountains.SetSeed(seed);
    hills.SetSeed(seed + 1);
    details.SetSeed(seed + 2);

    mountains.SetFrequency(0.002f);
    hills.SetFrequency(0.012f);
    details.SetFrequency(0.06f);

    const int baseHeight = 10;
    const int stoneLevel = 50;

    for (int x = 0; x < WORLD_SIZE; ++x) {
        float height =
            baseHeight
            + mountains.GetNoise(float(x), 0.0f) * 50.0f
            + hills.GetNoise(float(x), 0.0f) * 15.0f
            + details.GetNoise(float(x), 0.0f) * 3.0f;

        int surfaceY = std::clamp(
            static_cast<int>(height),
            0,
            WORLD_SIZE - 1
        );

        setBlock(BlockPosition{x, surfaceY, 1}, BlockID::GRASS);
        setBlock(BlockPosition{x, surfaceY, 0}, BlockID::GRASS);

        const auto stoneStart = stoneLevel + (int)(details.GetNoise(float(x), 0.0f) * 3.5f);
        for (int y = surfaceY + 1; y < WORLD_SIZE; ++y) {
            if (y < stoneStart) {
                setBlock(BlockPosition{x, y, 1}, BlockID::DIRT);
                setBlock(BlockPosition{x, y, 0}, BlockID::DIRT);
            } else if (y >= stoneStart) {
                setBlock(BlockPosition{x, y, 1}, BlockID::STONE);
                setBlock(BlockPosition{x, y, 0}, BlockID::STONE);
            } else if (y == WORLD_SIZE - 1) {
                setBlock(BlockPosition{x, y, 1}, BlockID::BEDROCK);
            }
        }
    }
}

void World::onTick() {
    m_time++;

    std::vector<Vec2i> unloadList;

    // for (auto& [pos, chunk] : m_chunks) {
    //     for (auto x = 0; x < CHUNK_WIDTH; x++) {
    //         for (auto y = 0; y < CHUNK_HEIGHT; y++) {
    //             auto block0 = chunk->getBlock({x, y, 0});
    //             if (block0 != nullptr) {
    //                 block0->onTick(this, {x, y, 0});
    //             }

    //             auto block1 = chunk->getBlock({x, y, 1});
    //             if (block1 != nullptr) {
    //                 block1->onTick(this, {x, y, 0});
    //             }
    //         }
    //     }

    //     for (int i = 0; i < m_randomTickSpeed; i++) {
    //         int x = rand() % CHUNK_WIDTH;
    //         int y = rand() % CHUNK_HEIGHT;
    //         uint8_t z = rand() % CHUNK_DEPTH;
    //         auto block = chunk->getBlock({x, y, z});
        
    //         if(block != nullptr) {
    //             block->onRandomTick(this, {x, y, z});
    //         }
    //     }

    //     bool shouldUnload = true;

    //     for (auto& [id, player] : m_players) {
    //         auto playerPos = player->getPosition();
    //         auto playerChunk = TO_CHUNK_POS(playerPos);
    //         shouldUnload &= playerChunk.distance(pos) > RENDER_DISTANCE;
    //     }

    //     if (shouldUnload) {
    //         unloadList.push_back(pos);
    //     }
    // }

    // for (auto& pos : unloadList) {
    //     saveChunk(pos);
    //     unloadChunk(pos);
    // }

    auto halfDistance = RENDER_DISTANCE / 2;

    for (auto& [id, player] : m_players) {
        player->onTick(*this);
        // auto playerPos = player->getPosition();
        // auto pos = TO_CHUNK_POS(playerPos);
        
        // for (int x = pos.x - halfDistance; x < pos.x + halfDistance; x++) {
        //     for (int y = pos.y - halfDistance; y < pos.y + halfDistance; y++) {
        //         auto chunk = getChunk({x, y});
                
        //         if (chunk == nullptr) {
        //             generateChunk({x, y});
        //             break;
        //         }
        //     }
        // }
    }

    for (auto& [playerId, info] : m_breakInfo) {
        auto player = m_players[playerId];
        auto block = getBlock(info.pos);
        auto tool = player->getItemHand();
        // auto toolType = (tool != nullptr) ? tool->getType() : TYPE_ITEM;
        // auto toolMaterial = (tool != nullptr) ? tool->getMaterial() : MATERIAL_NONE;
        float speed = 1.f;
        float d = Block::getDurability(block);

        // if (tool != nullptr && block->getBestTool() == toolType) {
        //     speed = tool->getToolSpeed();
        // }
    
        info.progress += speed / d * 0.01f;
    
        if (info.progress >= d || player->getGamemode() == GAMEMODE_CREATIVE) {
            if (player->getGamemode() != GAMEMODE_CREATIVE /* && toolMaterial >= block->getBestToolMaterial() */) {
                auto blockObject = Block::get(block);
                
                if (blockObject != nullptr) {
                    player->addItem(blockObject->dropItem(player->getItemHand()->item.get()));
                } else {
                    player->addItem(std::make_unique<Item>(block));
                }
            }
            
            destroyBlock(info.pos);
            player->setBreakingBlock(false);
        }
    }

    std::erase_if(m_breakInfo, [this](auto& info) {
        auto block = getBlock(info.second.pos);
        return block == BlockID::AIR;
    });
}

bool World::isBlockClosed(BlockPosition pos) {
    if (BlockID::AIR != getBlock({pos.x - 1, pos.y, pos.layer}) || 
        BlockID::AIR != getBlock({pos.x + 1, pos.y, pos.layer}) || 
        BlockID::AIR != getBlock({pos.x, pos.y - 1, pos.layer}) || 
        BlockID::AIR != getBlock({pos.x, pos.y + 1, pos.layer}) || 
        BlockID::AIR != getBlock({pos.x, pos.y, static_cast<uint8_t>(pos.layer ^ 1)})) {
        return false;
    }

    return true;
}

bool World::save() {
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

    return true;
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


    return true;
}

std::vector<Hitbox> World::getHitboxes(Hitbox entityHitbox, int radius) {
    int minX = entityHitbox.x - radius;
    int maxX = ceil(entityHitbox.x + entityHitbox.width * radius);
    int minY = entityHitbox.y - radius;
    int maxY = ceil(entityHitbox.y + entityHitbox.height * radius);

    std::vector<Hitbox> ret;

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            auto hitbox = getBlockHitbox({x, y, 1});

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

std::shared_ptr<Player> World::getPlayer(PlayerID id) {
    auto it = m_players.find(id);
    return (it != m_players.end()) ? it->second : nullptr;
}

void World::unloadPlayer(PlayerID id) {
    savePlayer(id);
    m_players.erase(id);
}

bool World::savePlayer(PlayerID id) {
    auto player = m_players[id];
    auto pbytes = player->serialize();

    if (!pbytes.size()) {
        logE("Failed to serialize player {}!", id);
    }

    auto username = player->getUsername();

    std::ofstream file(m_saveDir / "players" / ((username.empty() ? "player" : username) + ".dat"), std::ios::binary);
    
    if(file.is_open()) {
        file.write((char const*)pbytes.data(), pbytes.size());
        file.close();

        return true;
    }

    logE("Failed to save player {}!", id);
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

    return true;
}

bool World::isUsernameAlreadyTaken(std::string const& username) {
    auto it = std::find_if(m_players.begin(), m_players.end(), [username](auto&& pair) {
        return pair.second->getUsername() == username;
    });

    return it != m_players.end();
}

std::optional<Rectf> World::getBlockHitbox(BlockPosition pos) {
    if (pos.layer > 0) {
        auto block = getBlock(pos);

        if (block != BlockID::AIR) {
            return Rectf {static_cast<float>(pos.x), static_cast<float>(pos.y), 1.f, 1.f};
        } 
    }

    return std::nullopt;
}

bool World::isOutOfBound(BlockPosition pos) {
    return pos.x < 0 || pos.x >= WORLD_SIZE || pos.y < 0 || pos.y >= WORLD_SIZE && pos.layer < 0 && pos.layer >= WORLD_LAYERS;
}

void World::placeBlock(PlayerID m_id, BlockPosition pos) {
    auto player = m_players[m_id];
    auto slot = player->getItemHand();

    if (slot != nullptr && slot->item->isBlockItem() && getBlock(pos) == BlockID::AIR) {
        setBlock(pos, (BlockID)slot->item->getID());

        if(player->getGamemode() == GAMEMODE_SURVIVAL) {
            slot->count--;
            player->gc();
        }
    }
}

void World::breakBlock(PlayerID id, BlockPosition pos) {
    auto player = m_players[id];
    auto blockId = getBlock(pos);

    // Bedrock has -1.f
    if (Block::getDurability(blockId) < 0) {
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
    player->setBreakingBlock(true);
}

void World::stopBreakingBlock(PlayerID id) {
    m_players[id]->setBreakingBlock(false);
    m_breakInfo.erase(id);   
}