#include <cmath>
#include <fstream>
#include <filesystem>
#include <World.hpp>
#include <Logger.hpp>
#include <WorldGenFlat.hpp>
#include <WorldGenNormal.hpp>
#include <Chunk.hpp>
#include <SimplePlayer.hpp>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_header = WORLD;
}

World::~World() {}

void World::generate(WorldGen* generator) {
    m_WorldGen = generator;

    for(int x = 0; x < m_width / CHUNK_SIZE; x++) {
        auto chunk = new Chunk(this, x);
        chunk->generate();

        m_chunks.push_back(chunk);
    }
}

void World::update() {
    for(auto& [id, player] : m_players) {
        player->update();
    }
}

bool World::isBlockClosed(int x, int y, uint8_t l) {
    return getBlock(x - 1, y, l) && getBlock(x + 1, y, l) && getBlock(x, y - 1, l) && getBlock(x, y + 1, l);
}

void World::unloadChunk(int position) {
    for(auto i = m_chunks.begin(); i != m_chunks.end(); i++) {
        if((*i)->getPosition() == position) {
            delete *i;
            m_chunks.erase(i);
        }
    }
}

Chunk* World::getChunk(int position) {
    for(auto& chunk : m_chunks) {
        if(chunk->getPosition() == position) return chunk;
    }

    return nullptr;
}

void World::setChunk(Chunk* chunk) {
    auto worldChunk = getChunk(chunk->getPosition());
    if(worldChunk) {
        unloadChunk(chunk->getPosition());
    }

    m_chunks.push_back(chunk);
}

void World::placeBlock(int x, int y, uint8_t layer, enum Block::BlockType id) {
    if(getBlock(x, y, layer) == nullptr){
        setBlock(x, y, layer, new Block(id));
    }
}

void World::destroyBlock(int x, int y, uint8_t layer) {
    auto block = getBlock(x, y, layer);
    if(!block) return;

    setBlock(x, y, layer, nullptr);
}

Block *World::getBlock(int x, int y, uint8_t layer) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if(!chunk) return nullptr;

    return chunk->getBlock(x % CHUNK_WIDTH, y, layer);
}

void World::setBlock(int x, int y, uint8_t layer, Block* block) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if(!chunk) return;

    chunk->setBlock(x % 16, y, layer, block);
}

ByteVector& World::serialize() {
    SerializedObject::serialize();

    addBytes(m_width);
    addBytes(m_height);
    addBytes((unsigned int)m_chunks.capacity());

    for(int i = 0; i < m_chunks.capacity(); i++) {
        if(m_chunks[i] != nullptr) {
            auto chunkBytes = m_chunks[i]->serialize();

            addBytes((unsigned int)chunkBytes.size());
            addBytes(chunkBytes);
        }
    }

    return m_bytes;
}

int World::deserialize(ByteVector& bytes) {
    SerializedObject::deserialize(bytes);
    m_chunks.clear();

    m_width = getBytes<int>();
    m_height = getBytes<int>();
    int chunkCount = getBytes<unsigned int>();

    logD("Begin deserializing chunks");

    for(int i = 0; i < chunkCount; i++) {
        int chunkSize = getBytes<unsigned int>();
        auto chunk = new Chunk(this);

        ByteVector chunkBytes(m_bytes.begin() + m_offset, m_bytes.begin() + m_offset + chunkSize);
        m_offset += chunk->deserialize(chunkBytes);

        m_chunks.push_back(chunk);
    }

    logD("End deserializing chunks");

    return m_offset;
}

bool World::save() {
    auto worldData = this->serialize();
    
    std::ofstream file("myfile.bin", std::ios::binary);
    
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

std::vector<Rectf> World::getHitboxes(Rectf entityHitbox) {
    int minX = entityHitbox.x / BS  - 1;
    int maxX = (entityHitbox.x + entityHitbox.width) / BS + 1;
    int minY = entityHitbox.y / BS - 1;
    int maxY = (entityHitbox.y + entityHitbox.height) / BS + 1;

    std::vector<Rectf> ret;

    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            if(getBlock(x, y, 1) != nullptr) {
                ret.push_back(Rectf {x * BS, y * BS, BS, BS});
            }
        }
    }

    return ret;
}

void World::addPlayer(PlayerID id, SimplePlayer* player) {
    m_players.insert(std::make_pair(id, player));
}

PlayerID World::addPlayer(SimplePlayer* player) {
    addPlayer(m_lastPlayerID, player);
    auto ret = m_lastPlayerID;
    m_lastPlayerID++;
    return ret;
}

SimplePlayer* World::getPlayer(PlayerID id) {
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