#include <cmath>
#include <fstream>
#include <filesystem>
#include <World.hpp>
#include <Logger.hpp>
#include <WorldGenNormal.hpp>
#include <Chunk.hpp>
#include <SimplePlayer.hpp>
#include <assert.h>

World::World(uint32_t width, uint32_t height, std::string const& worldName) 
    : m_width(width), m_height(height), m_worldName(worldName) {
    m_header = WORLD;
}

World::World(std::string const& worldName) : World(0, 128, worldName) {}

World::~World() {}

void World::generate() {
    assert(("WorldGen isn't set!", m_worldGen != nullptr));
    auto start = static_cast<int32_t>(m_width > 0 ? -(m_width / CHUNK_WIDTH) / 2 : -3);
    auto end = static_cast<int32_t>(m_width > 0 ? (m_width / CHUNK_WIDTH) / 2 : 3);

    for(auto x = start; x < end; x++) {
        logD("gen chunk {}", x);
        m_chunks.insert(std::make_pair(x, m_worldGen->generateChunk(x)));
    }
}

void World::onTick() {
    for(auto& [id, player] : m_players) {
        player->onTick();
    }
}

bool World::isBlockClosed(int x, int y, uint8_t l) {
    return getBlock(x - 1, y, l) && getBlock(x + 1, y, l) && getBlock(x, y - 1, l) && getBlock(x, y + 1, l);
}

void World::unloadChunk(std::shared_ptr<Chunk> chunk) {
    if(chunk) {
        unloadChunk(chunk->getPosition());
    }
}

void World::unloadChunk(ChunkPos pos) {
    if(m_chunks.contains(pos)) {
        m_chunks.erase(pos);
    }
}

std::shared_ptr<Chunk> World::getChunk(ChunkPos position) {
    if(m_chunks.contains(position)) {
        return m_chunks[position];
    }

    return nullptr;
}

void World::addChunk(std::shared_ptr<Chunk> chunk) {
    if(!chunk) {
        return;
    }

    if(m_chunks.contains(chunk->getPosition())) {
        m_chunks.erase(chunk->getPosition());
    }

    m_chunks.insert(std::make_pair(chunk->getPosition(), chunk));
}

void World::placeBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> newBlock) {
    auto block = getBlock(x, y, layer);

    if(block && block->getID() == Block::ID::AIR) {
        setBlock(x, y, layer, newBlock);
    }
}

void World::destroyBlock(int32_t x, int32_t y, uint8_t layer) {
    logD("destroy {} {} block", x, y);
    auto block = getBlock(x, y, layer);

    if(block && block->getID() != Block::ID::AIR) {
        setBlock(x, y, layer, std::make_unique<Block>(Block::ID::AIR));
    }
}

std::shared_ptr<Block> World::getBlock(int32_t x, int32_t y, uint8_t layer) {
    if(isOutOfBound(x, y, layer)) {
        return nullptr;
    }
    
    auto chunk = getChunk(convertXtoChunkPosition(x));
    return chunk->getBlock(x % CHUNK_WIDTH, y, layer);
}

void World::setBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block) {
    if(!isOutOfBound(x, y, layer)) {
        auto chunk = getChunk(convertXtoChunkPosition(x));
        chunk->setBlock(x % CHUNK_WIDTH, y, layer, block);
    }
}

ByteVector& World::serialize() {
    SerializedObject::serialize();

    addBytes(WORLD_VERSION);
    addBytes(m_width);
    addBytes(m_height);
    addBytes(m_worldGen->getType());
    addBytes(m_worldGen->getSeed());

    // World version 1
    addBytes((uint32_t)m_chunks.size());

    logD("chunks count {}", m_chunks.size());

    for(auto [pos, chunk] : m_chunks) {
        if(chunk) {
            auto chunkBytes = chunk->serialize();

            addBytes((unsigned int)chunkBytes.size());
            addBytes(chunkBytes);
        }
    }

    return m_bytes;
}

size_t World::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);
    m_chunks.clear();

    auto worldVer = getBytes<uint32_t>();

    if(worldVer > WORLD_VERSION) {
        logE("Unsupported world version!");
        return m_offset;
    }

    m_width = getBytes<uint32_t>();
    m_height = getBytes<uint32_t>();

    auto generatorType = getBytes<WorldGen::Type>();
    auto seed = getBytes<int64_t>();

    // World version 1
    auto chunkCount = getBytes<unsigned int>();

    switch(generatorType) {
        default:
        case WorldGen::Type::NORMAL: {
            m_worldGen = std::make_shared<WorldGenNormal>(std::shared_ptr<World>(this), seed);
        }
    }

    for(int i = 0; i < chunkCount; i++) {
        int chunkSize = getBytes<unsigned int>();
        auto chunk = std::make_shared<Chunk>(std::shared_ptr<World>(this));

        ByteVector chunkBytes(m_bytes.begin() + m_offset, m_bytes.begin() + m_offset + chunkSize);
        m_offset += chunk->deserialize(chunkBytes);

        addChunk(chunk);
    }

    return m_offset;
}

bool World::save() {
    auto worldData = this->serialize();
    
    std::ofstream file("world.dat", std::ios::binary);
    
    if(!file.is_open()) return false;

    file.write((const char*)worldData.data(), worldData.size());
    return true;
}

bool World::load() {
    if(!std::filesystem::exists("world.dat")) return false;

    std::ifstream file("world.dat", std::ios::in | std::ios::binary);
    if (!file.is_open()) return false;

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

    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            auto block = getBlock(x, y, 1);

            if (block) {
                auto hitbox = block->getHitbox();

                if(hitbox.width != 0.0f && hitbox.height != 0.0f) {
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
    if(!m_players.count(id)) {
        return nullptr;
    }

    return m_players[id];
}

void World::unloadPlayer(PlayerID id) {
    m_players.erase(id);
}

bool World::isUsernameAlreadyTaken(std::string const& username) {
    for(auto& [_, player] : m_players) {
        if(player->getUsername() == username) return true;
    }

    return false;
}

Rectf World::getBlockHitbox(int x, int y) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if(!chunk) return Rectf {0.0f, 0.0f, 0.0f, 0.0f};

    return chunk->getBlock(x % CHUNK_WIDTH, y, 1)->getHitbox();
}

bool World::isOutOfBound(int x, int y, uint8_t layer) {
    if(m_width > 0 && (x < 0 || x > getWidth())) {
        return true;
    }

    if(!m_chunks.contains(convertXtoChunkPosition(x))) {
        return true;
    }
    
    return (y < 0 || y >= getHeight() || layer < 0 || layer > LAYERS - 1);
}