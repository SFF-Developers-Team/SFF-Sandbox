#include <cmath>
#include <fstream>
#include <filesystem>
#include <World.hpp>
#include <Logger.hpp>
#include <WorldGenNormal.hpp>
#include <Chunk.hpp>
#include <SimplePlayer.hpp>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_header = WORLD;
}

World::~World() {}

void World::generate(std::shared_ptr<WorldGen> generator) {
    m_worldGen = generator;

    for(int x = 0; x < m_width / CHUNK_SIZE; x++) {
        m_chunks.push_back(generator->generateChunk(x));
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
    m_chunks.erase(std::find(m_chunks.begin(), m_chunks.end(), chunk));
}

std::shared_ptr<Chunk> World::getChunk(int32_t position) {
    for(auto& chunk : m_chunks) {
        if(chunk->getPosition() == position) return chunk;
    }

    return nullptr;
}

void World::setChunk(std::shared_ptr<Chunk> chunk) {
    auto worldChunk = getChunk(chunk->getPosition());
    if(worldChunk) unloadChunk(worldChunk);

    m_chunks.push_back(chunk);
}

void World::placeBlock(int32_t x, int32_t y, uint8_t layer, enum Block::Type id) {
    auto block = getBlock(x, y, layer);
    if(block && block->getType() == Block::Type::AIR) setBlock(x, y, layer, std::make_unique<Block>(id));
}

void World::destroyBlock(int32_t x, int32_t y, uint8_t layer) {
    auto block = getBlock(x, y, layer);
    if(!block || block->getType() == Block::Type::AIR) return;

    setBlock(x, y, layer, std::make_unique<Block>(Block::Type::AIR));
}

std::shared_ptr<Block> World::getBlock(int32_t x, int32_t y, uint8_t layer) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if(!chunk) return nullptr;

    return chunk->getBlock(x % CHUNK_WIDTH, y, layer);
}

void World::setBlock(int32_t x, int32_t y, uint8_t layer, std::shared_ptr<Block> block) {    
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if(!chunk) return;

    chunk->setBlock(x % 16, y, layer, block);
}

ByteVector& World::serialize() {
    SerializedObject::serialize();

    addBytes(m_width);
    addBytes(m_height);
    addBytes((uint32_t)m_chunks.size());

    logD("chunks count {}", m_chunks.size());

    for(auto chunk : m_chunks) {
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

    m_width = getBytes<int>();
    m_height = getBytes<int>();
    int chunkCount = getBytes<unsigned int>();

    logD("Begin deserializing chunks");

    for(int i = 0; i < chunkCount; i++) {
        int chunkSize = getBytes<unsigned int>();
        auto chunk = std::make_shared<Chunk>(this);

        ByteVector chunkBytes(m_bytes.begin() + m_offset, m_bytes.begin() + m_offset + chunkSize);
        m_offset += chunk->deserialize(chunkBytes);

        m_chunks.push_back(chunk);
    }

    logD("End deserializing chunks");

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
            if(!block) continue;
            
            auto hitbox = getBlockHitbox(x, y);
            if(hitbox.width == 0.0f || hitbox.height == 0.0f) continue;
            ret.push_back(hitbox);
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
    return (x < 0 || x > getWidth() || y < 0 || y >= getHeight() || layer < 0 || layer > LAYERS - 1);
}