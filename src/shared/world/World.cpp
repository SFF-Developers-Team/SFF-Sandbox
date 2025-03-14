#include <world/gen/WorldGenNormal.hpp>
#include <entity/SimplePlayer.hpp>
#include <world/World.hpp>
#include <world/Chunk.hpp>
#include <Logger.hpp>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <assert.h>
#include <Logger.hpp>
#include <world/blocks/Leaves.hpp>

World::World(std::filesystem::path const& saveDir) : m_saveDir(saveDir), m_version(WORLDVER), m_time(0), m_lastPlayerID(1), m_randomTickSpeed(3) {
    if (!saveDir.empty() && !std::filesystem::exists(m_saveDir)) {
        std::filesystem::create_directory(m_saveDir);
        std::filesystem::create_directory(m_saveDir / "chunks");
        std::filesystem::create_directory(m_saveDir / "players");
    }
}

void World::generate() {
    // spawn area [-3; 3] x [-3; 3]

    for (auto i = -9; i < 9; i++) {
        generateChunk({i / 3, i % 3});
    }
}

void World::onTick() {
    m_time++;

    for (auto& [pos, chunk] : m_chunks) {
        for (auto x = 0; x < CHUNK_WIDTH; x++) {
            for (uint8_t y = 0; x < CHUNK_HEIGHT; y++) {
                chunk->getBlock({x, y, 0})->onTick(this, {x, y, 0});
                chunk->getBlock({x, y, 1})->onTick(this, {x, y, 1});
            }
        }

        for (int i = 0; i < m_randomTickSpeed; i++) {
            int x = rand() % CHUNK_WIDTH;
            int y = rand() % CHUNK_HEIGHT;
            uint8_t z = rand() % CHUNK_DEPTH;
            auto block = chunk->getBlock({x, y, z});
        
            if(block) {
                block->onRandomTick(this, {x, y, z});
            }
        }
    }

    for (auto& [id, player] : m_players) {
        player->onTick(this);
        auto playerPos = player->getPosition();
        auto pos = TO_CHUNK_POS(playerPos);
        
        for (int i = -4; i < 4; i++) {
            auto chunk = getChunk({i / 2, i % 2});
            
            if (chunk == nullptr) {
                generateChunk({i / 2, i % 2});
            }
        }
    }
}

bool World::isBlockClosed(BlockPosition pos) {
    if (!getBlock({pos.x - 1, pos.y, pos.layer}) || 
        !getBlock({pos.x + 1, pos.y, pos.layer}) || 
        !getBlock({pos.x, pos.y - 1, pos.layer}) || 
        !getBlock({pos.x, pos.y + 1, pos.layer}) || 
        !getBlock({pos.x, pos.y, static_cast<uint8_t>(pos.layer ^ 1)})) {
        return false;
    }

    return true;
}

std::shared_ptr<Chunk> World::getChunk(Vec2i position) {
    auto it = m_chunks.find(position);
    return (it != m_chunks.end()) ? it->second : nullptr;
}

bool World::saveChunk(Vec2i pos) {
    auto chunk = m_chunks[pos];
    auto cbytes = chunk->serialize();

    if (!cbytes.size()) {
        logE("Failed to serialize chunk {},{}!", pos.x, pos.y);
    }

    std::ofstream file(m_saveDir / "chunks" / std::format("{},{}.dat", pos.x, pos.y), std::ios::binary);
    
    if(file.is_open()) {
        file.write((char const*)cbytes.data(), cbytes.size());
        file.close();

        return true;
    }

    logE("Failed to save chunk {},{}!", pos.x, pos.y);
    return false;
}

bool World::loadChunk(Vec2i pos) {
    auto chunkIt = m_chunks.find(pos);

    if (chunkIt == m_chunks.end()) {
        chunkIt->second = std::make_shared<Chunk>();
    }
    
    std::ifstream file(m_saveDir / "chunks" / std::format("{},{}.dat", pos.x, pos.y), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        logE("Can't load chunk {},{}!", pos.x, pos.y);
        return false;
    }

    file.seekg(0, std::ios::end);
    int chunkSize = file.tellg();

    file.seekg(0, std::ios::beg);
    auto chunkBytes = DataStream(chunkSize);

    file.read((char*)chunkBytes.data(), chunkSize);
    chunkIt->second->deserialize(chunkBytes);
        
    return true;
}

std::shared_ptr<Block> World::getBlock(BlockPosition pos) {
    if (isOutOfBound(pos)) {
        return nullptr;
    }

    auto chunk = getChunk(TO_CHUNK_POS(pos));
    return chunk->getBlock({pos.x % CHUNK_WIDTH, pos.y % CHUNK_HEIGHT, pos.layer});
}

void World::setBlock(BlockPosition pos, std::shared_ptr<Block> block) {
    if (isOutOfBound(pos) || (block && block->getID() == ItemID::AIR)) {
        return;
    }

    auto chunk = getChunk(TO_CHUNK_POS(pos));
    auto localx = (pos.x < 0 ? (CHUNK_WIDTH - (-pos.x) % CHUNK_WIDTH) % CHUNK_WIDTH : pos.x % CHUNK_WIDTH);
    
    chunk->setBlock({localx, pos.y % CHUNK_HEIGHT, pos.layer}, block);
}

bool World::save() {
    if (!std::filesystem::exists(m_saveDir) && !std::filesystem::create_directory(m_saveDir)) {
        logE("Failed to save world!");
        return false;
    }

    for (auto [pos, _] : m_chunks) {
        saveChunk(pos);
    }

    for (auto [id, _] : m_players) {
        savePlayer(id);
    }

    auto worldData = DataStream();
    worldData.add(WORLD_VERSION);
    worldData.add(m_worldGen->getType());
    worldData.add(m_worldGen->getSeed());
    worldData.add(m_time);

    auto worldFile = std::ofstream(m_saveDir / "world.dat", std::ios::binary);
    if(!worldFile.is_open()) {
        logE("Failed to save world data!");
        return false;
    }
    
    worldFile.write((char const*)worldData.data(), worldData.size());

    return true;
}

bool World::load() {
    if (!std::filesystem::exists(m_saveDir)) {
        logE("World directory not found!");
        return false;
    }

    if (!std::filesystem::exists(m_saveDir / "world.dat")) {
        logE("world.dat not found!");
        return false;
    }

    std::ifstream file(m_saveDir / "world.dat", std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        logE("Can't open world.dat");
        return false;
    }

    file.seekg(0, std::ios::end);
    int worldSize = file.tellg();

    file.seekg(0, std::ios::beg);
    auto worldData = DataStream(worldSize);

    file.read((char*)worldData.data(), worldData.size());

    m_version = worldData.get<uint32_t>();

    if (m_version == 256 && worldData.getI<uint32_t>() == 128) {
        logE("Old world version detected! We don't support it now...");
        return false;
    }

    // We have only one world gen type now...
    auto genType = worldData.get<WorldGenType>();
    auto seed = worldData.get<uint64_t>();

    m_worldGen = std::make_shared<WorldGenNormal>(std::shared_ptr<World>(this), seed);
    m_time = worldData.get<uint64_t>();

    for (const auto& entry : std::filesystem::directory_iterator(m_saveDir / "chunks")) {
        std::stringstream ss(entry.path().stem().string());
        std::string part;
        Vec2i chunkPosition;
    
        std::getline(ss, part, ',');
        chunkPosition.x = std::stoi(part);
        std::getline(ss, part, ',');
        chunkPosition.y = std::stoi(part);
    
    
        loadChunk(chunkPosition);
    }


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

            if (hitbox.width > 0.f && hitbox.height > 0.f) {
                ret.push_back(hitbox);
            }
        }
    }

    return ret;
}

void World::setPlayer(PlayerID id, std::shared_ptr<SimplePlayer> player, std::string const username) {
    m_players[id] = player;
    player->setUsername(username);
}

PlayerID World::addPlayer(std::shared_ptr<SimplePlayer> player, std::string const username) {
    auto it = std::find_if(m_players.begin(), m_players.end(), [username](auto& pair) { 
        return pair.second->getUsername() == username;
    });

    if (it != m_players.end()) {
        return 0;
    }

    it->second = player;
    player->setID(m_lastPlayerID);
    player->setUsername(username);

    logD("Added player with id {}", player->getPlayerID());

    return m_lastPlayerID++;
}

std::shared_ptr<SimplePlayer> World::getPlayer(PlayerID id) {
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
    auto it = std::find_if(m_players.begin(), m_players.end(), [username](auto&& pair) {
        return pair.second->getUsername() == username;
    });

    if (it == m_players.end()) {
        it->second = std::make_shared<SimplePlayer>(std::shared_ptr<World>(this));
    }

    std::ifstream file(m_saveDir / "players" / ((username.empty() ? "player" : username) + ".dat"), std::ios::in | std::ios::binary);
        
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, std::ios::end);
    int worldSize = file.tellg();

    file.seekg(0, std::ios::beg);
    auto playerBytes = DataStream(worldSize);

    file.read((char*)playerBytes.data(), playerBytes.size());
    file.close();

    it->second->deserialize(playerBytes);
    it->second->setUsername(username);

    return true;
}

bool World::isUsernameAlreadyTaken(std::string const& username) {
    auto it = std::find_if(m_players.begin(), m_players.end(), [username](auto&& pair) {
        return pair.second->getUsername() == username;
    });

    return it != m_players.end();
}

Rectf World::getBlockHitbox(BlockPosition pos) {
    if (pos.layer > 0) {
        auto block = getBlock(pos);

        if (block != nullptr) {
            return Rectf {static_cast<float>(pos.x), static_cast<float>(pos.y), 1.f, 1.f};
        } 
    }

    return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
}

bool World::isOutOfBound(BlockPosition pos) {
    if (!m_chunks.contains(TO_CHUNK_POS(pos))) {
        return true;
    }

    return (pos.y < 0 || pos.layer > CHUNK_DEPTH - 1);
}

void World::generateChunk(Vec2i pos) {
    if (m_worldGen == nullptr) {
        return logE("Can't generate chunk without WorldGen ¯\\_(ツ)_/¯");
    }

    m_chunks[pos] = m_worldGen->generateChunk(pos);

    for (auto& tree : m_postGenTrees) {
        for (auto y = tree.y - tree.trunkHeight; y < tree.y; y++) {
            auto log = Block::create(OAK_LOG);
            log->setTag(TAG_NATURAL, true);

            setBlock({tree.x, y, 0}, log);
        }

        for (int x = -3; x <= 3; ++x) {
            for (int y = -2; y <= 2; ++y) {
                if (x * x + y * y <= 3 * 2) {
                    Vec2i pos = {tree.x + x, (tree.y - tree.trunkHeight) + y};
                    
                    auto chunkPos = TO_CHUNK_POS(pos);

                    if (getChunk(chunkPos) == nullptr) {
                        generateChunk(chunkPos);
                    }

                    auto leaves = Block::create(LEAVES);

                    if (getBlock({pos.x, pos.y, 0}) == nullptr) {
                        setBlock({pos.x, pos.y, 0}, Block::create(*leaves));
                    }

                    setBlock({pos.x, pos.y, 1}, leaves);
                }
            }
        }
    }

    m_postGenTrees.clear();
}