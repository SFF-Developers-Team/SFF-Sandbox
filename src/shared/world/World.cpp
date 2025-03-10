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

World::World(uint32_t height, std::filesystem::path const& saveDir) : m_height(height), m_saveDir(saveDir), m_version(WORLDVER), m_time(0), m_lastPlayerID(1) {
    if (!saveDir.empty() && !std::filesystem::exists(m_saveDir)) {
        std::filesystem::create_directory(m_saveDir);
        std::filesystem::create_directory(m_saveDir / "chunks");
        std::filesystem::create_directory(m_saveDir / "players");
    }
}

World::World(std::filesystem::path const& saveDir) : World(256, saveDir) {}

void World::generate() {
    if (m_worldGen == nullptr) {
        return logE("Can't generate world without WorldGen ¯\\_(ツ)_/¯");
    }

    // spawn area [-3; 3]
    for (auto x = -3; x < 3; x++) {
        generateChunk(x);
    }
}

void World::onTick() {
    m_time++;

    for (auto& [x, chunk] : m_chunks) {
        chunk->onTick();
    }

    for (auto& [id, player] : m_players) {
        player->onTick();
    }
}

bool World::isBlockClosed(int x, int y, uint8_t l) {
    return getBlock(x - 1, y, l) && getBlock(x + 1, y, l) && getBlock(x, y - 1, l) && getBlock(x, y + 1, l);
}

void World::unloadChunk(ChunkPosition pos) {
    if (m_chunks.contains(pos)) {
        m_chunks.erase(pos);
    }
}

std::shared_ptr<Chunk> World::getChunk(ChunkPosition position) {
    if (m_chunks.contains(position)) {
        return m_chunks[position];
    }

    return nullptr;
}

void World::addChunk(std::shared_ptr<Chunk> chunk) {
    m_chunks[chunk->getPosition()] = chunk;
}

bool World::saveChunk(ChunkPosition pos) {
    auto chunk = m_chunks[pos];
    auto cbytes = chunk->serialize();

    if (!cbytes.size()) {
        logE("Failed to serialize chunk {}!", pos);
    }

    std::ofstream file(m_saveDir / "chunks" / (std::to_string(pos) + ".dat"), std::ios::binary);
    
    if(file.is_open()) {
        file.write((char const*)cbytes.data(), cbytes.size());
        file.close();

        return true;
    }

    logE("Failed to save chunk {}!", pos);
    return false;
}

bool World::loadChunk(ChunkPosition pos) {
    auto chunkIt = m_chunks.find(pos);

    if (chunkIt == m_chunks.end()) {
        m_chunks[pos] = std::make_shared<Chunk>(std::shared_ptr<World>(this));
    }
    
    std::ifstream file(m_saveDir / "chunks" / (std::to_string(pos) + ".dat"), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        logE("Can't load chunk {}", pos);
        return false;
    }

    file.seekg(0, std::ios::end);
    int chunkSize = file.tellg();

    file.seekg(0, std::ios::beg);
    auto chunkBytes = DataStream(chunkSize);

    file.read((char*)chunkBytes.data(), chunkSize);
    m_chunks[pos]->deserialize(chunkBytes);
        
    return true;
}

void World::placeBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> newBlock) {
    if (!getBlock(x, y, layer)) {
        setBlock(x, y, layer, newBlock);
    }
}

void World::destroyBlock(int32_t x, int32_t y, uint8_t layer) {
    if (getBlock(x, y, layer)) {
        setBlock(x, y, layer, nullptr);
    }
}

std::shared_ptr<Block> World::getBlock(int32_t x, int32_t y, uint8_t layer) {
    if (isOutOfBound(x, y, layer)) {
        return nullptr;
    }

    auto chunk = getChunk(xToChunk(x));
    return chunk->getBlock(x % CHUNK_WIDTH, y, layer);
}

void World::setBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block, bool natural) {
    if (isOutOfBound(x, y, layer) || (block && block->getID() == ItemID::AIR)) {
        return;
    }

    if (natural) {
        block->setTag(TAG_NATURAL, true);
    }

    auto chunk = getChunk(xToChunk(x));
    auto localx = (x < 0 ? (CHUNK_WIDTH - (-x) % CHUNK_WIDTH) % CHUNK_WIDTH : x % CHUNK_WIDTH);
    
    chunk->setBlock(localx, y, layer, block);
}
#if 0
ByteVector World::serialize() {
    DataStream::serialize();

    add(WORLD_VERSION);
    add(m_height);
    add(m_worldGen->getType());
    add(m_worldGen->getSeed());
    // World version 3
    add(m_time);

    return bytes();
}

size_t World::deserialize(ByteVector const& bytes) {
    DataStream::deserialize(bytes);
    m_chunks.clear();

    m_version = get<uint32_t>();
    m_height = get<uint32_t>();

    bool oldVersion = (int)(m_version) == 256 && (int)(m_height) == 128;

    if (!oldVersion) {
        if (m_version > WORLD_VERSION) {
            logE("Unsupported world version! (World version: {} | Supported: {})", m_version, WORLD_VERSION);
            return m_offset;
        }

        auto generatorType = get<WorldGen::Type>();
        auto seed = get<int64_t>();

        if(m_version >= 3) {
            m_time = get<uint64_t>(0);
        }

        switch (generatorType) {
            default:
            case WorldGen::Type::NORMAL: {
                m_worldGen = std::make_shared<WorldGenNormal>(std::shared_ptr<World>(this), seed);
            }
        }
    } else {
        logE("Old world version detected.");
        m_version = 0;
        m_worldGen = std::make_shared<WorldGenNormal>(std::shared_ptr<World>(this), 0);
    }

    logD("World version {}", m_version);

    // World version 1-3
    if (m_version < 4) {
        auto chunkCount = get<uint32_t>();
        while (chunkCount-- > 0) {
            auto csize = get<uint32_t>();
            auto cbytes = getN(csize);

            auto chunk = std::make_shared<Chunk>(std::shared_ptr<World>(this));
            chunk->deserialize(cbytes);

            addChunk(chunk);
        }
    }
    // World end

    return m_offset;
}
#endif

bool World::save() {
    // if (!std::filesystem::exists(m_saveDir) && !std::filesystem::create_directory(m_saveDir)) {
    //     logE("Failed to save world!");
    //     return false;
    // }

    // for (auto [x, chunk] : m_chunks) {
    //     saveChunk(x);
    // }

    // for (auto [id, player] : m_players) {
    //     savePlayer(id);
    // }

    // auto worldData = this->serialize();

    // if (!worldData.size()) {
    //     logE("Failed to serialize world!");
    //     return false;
    // }

    // std::ofstream file(m_saveDir / "world.dat", std::ios::binary);

    // if (!file.is_open()) {
    //     logE("Failed to save world!");
    //     return false;
    // }

    // file.write((char const*)worldData.data(), worldData.size());

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
    // auto worldBytes = ByteVector(worldSize);

    // file.read((char*)worldBytes.data(), worldBytes.size());
    // deserialize(worldBytes);

    // if (m_version >= 4) {
    //     for (const auto& entry : std::filesystem::directory_iterator(m_saveDir / "chunks")) {
    //         auto filename = entry.path().stem().string();
            
    //         if (!filename.empty() && std::all_of(filename.begin(), filename.end(), ::isdigit)) {
    //             loadChunk(std::stoi(filename));
    //         }
    //     }
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
            auto block = getBlock(x, y, 1);

            if (block) {
                auto hitbox = block->getHitbox();

                if (hitbox.width != 0.0f && hitbox.height != 0.0f) {
                    ret.push_back(hitbox);
                }
            }
        }
    }

    return ret;
}

void World::addPlayer(PlayerID id, std::shared_ptr<SimplePlayer> player, std::string const username) {
    m_players.insert(std::make_pair(id, player));
    player->setID(id);
    player->setUsername(username);

    logD("Added player with id {}", player->getPlayerID());
}

PlayerID World::addPlayer(std::shared_ptr<SimplePlayer> player, std::string const username) {
    addPlayer(m_lastPlayerID, player, username);
    auto ret = m_lastPlayerID;
    m_lastPlayerID++;

    return ret;
}

std::shared_ptr<SimplePlayer> World::getPlayer(PlayerID id) {
    if (!m_players.contains(id)) {
        return nullptr;
    }

    return m_players[id];
}

void World::unloadPlayer(PlayerID id) {
    m_players.erase(id);
}

bool World::savePlayer(PlayerID id) {
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
    // auto playerIt = std::find_if(m_players.begin(), m_players.end(), [username](auto&& pair) {
    //     return pair.second->getUsername() == username;
    // });

    // auto player = (playerIt != m_players.end() ? playerIt->second : std::make_shared<SimplePlayer>(std::shared_ptr<World>(this)));

    // std::ifstream file(m_saveDir / "players" / ((username.empty() ? "player" : username) + ".dat"), std::ios::in | std::ios::binary);
        
    // if (!file.is_open()) {
    //     return false;
    // }

    // file.seekg(0, std::ios::end);
    // int worldSize = file.tellg();

    // file.seekg(0, std::ios::beg);
    // auto playerBytes = ByteVector(worldSize);

    // file.read((char*)playerBytes.data(), playerBytes.size());
    // file.close();

    // player->deserialize(playerBytes);
    // player->setUsername(username);

    return true;
}

bool World::isUsernameAlreadyTaken(std::string const& username) {
    for (auto& [_, player] : m_players) {
        if (player->getUsername() == username)
            return true;
    }

    return false;
}

Rectf World::getBlockHitbox(int x, int y) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    
    if (chunk != nullptr) {
        return chunk->getBlock(x % CHUNK_WIDTH, y, 1)->getHitbox();
    }

    return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
}

bool World::isOutOfBound(int x, int y, uint8_t layer) {
    if (!m_chunks.contains(xToChunk(x))) {
        return true;
    }

    return (y < 0 || y >= getHeight() || layer < 0 || layer > LAYERS - 1);
}

void World::generateChunk(ChunkPosition xPos) {
    m_chunks.insert(std::make_pair(xPos, m_worldGen->generateChunk(xPos)));

    for (auto& tree : m_postGenTrees) {
        for (auto y = tree.y - tree.trunkHeight; y < tree.y; y++) {
            setBlock(tree.x, y, 0, Block::create(OAK_LOG), true);
        }

        for (int x = -3; x <= 3; ++x) {
            for (int y = -2; y <= 2; ++y) {
                if (x * x + y * y <= 3 * 2) {
                    Vec2i pos = {tree.x + x, (tree.y - tree.trunkHeight) + y};
                    
                    auto xChunk = xToChunk(pos.x);

                    if (getChunk(xChunk) == nullptr) {
                        generateChunk(xChunk);
                    }

                    if (getBlock(pos.x, pos.y, 0) == nullptr) {
                        setBlock(pos.x, pos.y, 0, Block::create(LEAVES), true);
                    }

                    setBlock(pos.x, pos.y, 1, Block::create(LEAVES), true);
                }
            }
        }
    }

    m_postGenTrees.clear();
}