#include <world/gen/WorldGenNormal.hpp>
#include <entity/SimplePlayer.hpp>
#include <world/World.hpp>
#include <world/Chunk.hpp>
#include <Logger.hpp>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <assert.h>

World::World(uint32_t height, std::string const& worldName) : m_height(height), m_worldName(worldName), m_version(WORLDVER), m_spentTime(0), m_loadTime(0) {
    m_header = WORLD;
}

World::World(std::string const& worldName) : World(128, worldName) {}

void World::generate() {
    assert(m_worldGen != nullptr);
    int start = -3;
    int end = 3;

    for (auto x = start; x < end; x++) {
        logD("gen chunk {}", x);
        m_chunks.insert(std::make_pair(x, m_worldGen->generateChunk(x)));
    }
}

void World::reset() {
    m_chunks.clear();
    m_players.clear();
    m_worldName.clear();
    m_worldGen = nullptr;
    m_version = WORLDVER;
    m_lastPlayerID = 1;
    m_spentTime = 0;
    m_loadTime = 0;
}

void World::onTick() {
    for (auto& [id, player] : m_players) {
        player->onTick();
    }
}

bool World::isBlockClosed(int x, int y, uint8_t l) {
    return getBlock(x - 1, y, l) && getBlock(x + 1, y, l) && getBlock(x, y - 1, l) && getBlock(x, y + 1, l);
}

void World::unloadChunk(std::shared_ptr<Chunk> chunk) {
    if (chunk) {
        unloadChunk(chunk->getPosition());
    }
}

void World::unloadChunk(Chunk::Position pos) {
    if (m_chunks.contains(pos)) {
        m_chunks.erase(pos);
    }
}

std::shared_ptr<Chunk> World::getChunk(Chunk::Position position) {
    if (m_chunks.contains(position)) {
        return m_chunks[position];
    }

    return nullptr;
}

void World::addChunk(std::shared_ptr<Chunk> chunk) {
    m_chunks[chunk->getPosition()] = chunk;
}

void World::placeBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> newBlock) {
    auto block = getBlock(x, y, layer);

    if (block && block->getID() == Block::ID::AIR) {
        setBlock(x, y, layer, newBlock);
    }
}

void World::destroyBlock(int32_t x, int32_t y, uint8_t layer) {
    auto block = getBlock(x, y, layer);

    if (block && block->getID() != Block::ID::AIR) {
        setBlock(x, y, layer, std::make_unique<Block>(Block::ID::AIR));
    }
}

std::shared_ptr<Block> World::getBlock(int32_t x, int32_t y, uint8_t layer) {
    if (isOutOfBound(x, y, layer)) {
        return nullptr;
    }

    auto chunk = getChunk(xToChunk(x));
    return chunk->getBlock(x % CHUNK_WIDTH, y, layer);
}

void World::setBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block) {
    if (!isOutOfBound(x, y, layer)) {
        auto chunk = getChunk(xToChunk(x));

        auto localx = (x < 0 ? (CHUNK_WIDTH - (-x) % CHUNK_WIDTH) % CHUNK_WIDTH : x % CHUNK_WIDTH);
        chunk->setBlock(localx, y, layer, block);
    }
}

ByteVector World::serialize() {
    SerializedObject::serialize();

    add(WORLD_VERSION);
    add(m_height);
    add(m_worldGen->getType());
    add(m_worldGen->getSeed());
    // World version 3
    add(m_spentTime + (std::time(NULL) - m_loadTime));

    // World version 1
    add((uint32_t)m_chunks.size());
    for (auto [_, chunk] : m_chunks) {
        if (chunk != nullptr) {
            auto cbytes = chunk->serialize();

            if (!cbytes.size()) {
                logE("Failed to save chunk");
                reset();

                return bytes();
            }

            add<uint32_t>(cbytes.size());
            add(cbytes);
        }
    }

    // World end

    return bytes();
}

size_t World::deserialize(ByteVector const& bytes) {
    SerializedObject::deserialize(bytes);
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
            m_spentTime = get<uint64_t>(0);
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

    // World version 1
    auto chunkCount = get<uint32_t>();
    while (chunkCount-- > 0) {
        auto csize = get<uint32_t>();
        auto cbytes = getN(csize);

        auto chunk = std::make_shared<Chunk>(std::shared_ptr<World>(this));
        chunk->deserialize(cbytes);

        addChunk(chunk);
    }

    // World end

    m_loadTime = std::time(NULL);

    return m_offset;
}

bool World::save() {
    auto worldData = this->serialize();

    if (!worldData.size()) {
        logE("Failed to save world");
        return false;
    }

    std::ofstream file("world.dat", std::ios::binary);

    if (!file.is_open()) {
        logE("Failed to open save file");
        return false;
    }

    file.write((char const*)worldData.data(), worldData.size());

    return true;
}

bool World::load() {
    if (!std::filesystem::exists("world.dat"))
        return false;

    std::ifstream file("world.dat", std::ios::in | std::ios::binary);
    if (!file.is_open())
        return false;

    file.seekg(0, std::ios::end);
    int worldSize = file.tellg();

    file.seekg(0, std::ios::beg);
    auto worldBytes = ByteVector(worldSize);

    file.read((char*)worldBytes.data(), worldBytes.size());
    deserialize(worldBytes);

    return true;
}

std::vector<Hitbox> World::getHitboxes(Hitbox entityHitbox) {
    int minX = entityHitbox.x - 1;
    int maxX = ceil(entityHitbox.x + entityHitbox.width);
    int minY = entityHitbox.y - 1;
    int maxY = ceil(entityHitbox.y + entityHitbox.height);

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

    logD("Added player with id {}", player->getID());
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

bool World::isUsernameAlreadyTaken(std::string const& username) {
    for (auto& [_, player] : m_players) {
        if (player->getUsername() == username)
            return true;
    }

    return false;
}

Rectf World::getBlockHitbox(int x, int y) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if (!chunk)
        return Rectf {0.0f, 0.0f, 0.0f, 0.0f};

    return chunk->getBlock(x % CHUNK_WIDTH, y, 1)->getHitbox();
}

bool World::isOutOfBound(int x, int y, uint8_t layer) {
    if (!m_chunks.contains(xToChunk(x))) {
        return true;
    }

    return (y < 0 || y >= getHeight() || layer < 0 || layer > LAYERS - 1);
}