#include <World.hpp>
#include <Player.hpp>
#include <cmath>
#include <Debug.hpp>
#include <WorldGenFlat.hpp>
#include <WorldGenNormal.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <fstream>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_player = new Player(this);
    m_header = WORLD;

    calcLightDepths();
}

World::~World() {
    delete m_player;
}

void World::generate(WorldGen* generator) {
    m_WorldGen = generator;

    for(int x = 0; x < m_width / CHUNK_SIZE; x++) {
        auto chunk = new Chunk(this, x);
        chunk->generate();

        m_chunks.push_back(chunk);
    }

    calcLightDepths();
}

void World::update(int renderDistance) {
    auto pp = m_player->getPosition();
    m_hitboxes.clear();

    for (int x = (int)(pp.x / BLOCK_SIZE_PIXELS) - 5; x < (int)(pp.x / BLOCK_SIZE_PIXELS) + 5; x++) {
        for(int y = (int)(pp.y / BLOCK_SIZE_PIXELS) - 5; y < (int)(pp.y / BLOCK_SIZE_PIXELS) + 5; y++) {
            if(getBlock(x, y) != nullptr) {
                m_hitboxes.push_back(Rectangle {(float)x * BLOCK_SIZE_PIXELS, (float)y * BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS});
            }
        }
    }

    m_player->update(m_hitboxes);

    Debug::addString(TextFormat("World size: %dx%d", m_width, m_height));
}

void World::draw(bool debug) {
    int chunksDrawn = 0;

    for (auto& chunk : m_chunks) {
        if(m_player->isChunkInView(chunk)) {
            chunk->draw();
            chunksDrawn++;
        }

        auto playerTarget = m_player->getTargetBlock();

        // Selected block
        if(playerTarget.x > 0 && playerTarget.y > 0) {
            DrawRectangleLinesEx({playerTarget.x * BS, playerTarget.y * BS, BS, BS}, 1.0f, WHITE);
        }

        if(Debug::m_debug) {
            DrawLineV({
                (float)chunk->getPosition() * CHUNK_SIZE * BLOCK_SIZE_PIXELS, 0}, 
                {(float)chunk->getPosition() * CHUNK_SIZE * BLOCK_SIZE_PIXELS, (float)m_height * BLOCK_SIZE_PIXELS}, YELLOW
            );

            DrawLineV({
                (float)chunk->getPosition() * (CHUNK_SIZE * 2) * BLOCK_SIZE_PIXELS, 0}, 
                {(float)chunk->getPosition() * (CHUNK_SIZE * 2) * BLOCK_SIZE_PIXELS, (float)m_height * BLOCK_SIZE_PIXELS}, YELLOW
            );
        
            for(auto& hitbox : m_hitboxes) {
                DrawRectangleLinesEx(hitbox, 1.0f, RED);
            }
        }
    }

    Debug::addString(TextFormat("Chunks drawn: %d", chunksDrawn));

    m_player->draw();
}

bool World::isBlockClosed(int x, int y) {
    return getBlock(x - 1, y) && getBlock(x + 1, y) && getBlock(x, y - 1) && getBlock(x, y + 1);
}

void World::calcLightDepths() {
    for(auto& chunk : m_chunks) {
        chunk->resetLightDepts();

        for(int x = 0; x < CHUNK_WIDTH; x++) {
            for(int y = 0; y < m_height; y++) {
                int d = 1;
                auto absX = chunk->getPosition() * CHUNK_WIDTH + x;

                while(chunk->getLightDepth(x, y) > -1.0f) {
                    if (getBlock(absX - d, y) != nullptr && 
                        getBlock(absX + d, y) != nullptr && 
                        getBlock(absX, y - d) != nullptr && 
                        getBlock(absX, y + d) != nullptr
                    ) {
                        chunk->setLightDepth(x, y, chunk->getLightDepth(x, y) - 0.4f);
                        d++;
                    } else {
                        break;
                    }
                }
            }
        }
    }
}

Chunk* World::getChunk(int position) {
    for(auto& chunk : m_chunks) {
        if(chunk->getPosition() == position) return chunk;
    }

    return nullptr;
}

void World::placeBlock(int x, int y, enum Block::BlockType id) {
    int index = x * m_height + y;

    if(getBlock(x, y) == nullptr){
        setBlock(x, y, new Block(id));
        calcLightDepths();
    }
}

void World::destroyBlock(int x, int y) {
    int index = x * m_height + y;

    setBlock(x, y, nullptr);
    calcLightDepths();
}

Block *World::getBlock(int x, int y) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if(!chunk) return nullptr;

    return chunk->getBlock(x % CHUNK_WIDTH, y);
}

void World::setBlock(int x, int y, Block* block) {
    auto chunk = getChunk(x / CHUNK_WIDTH);
    if(!chunk) return;

    chunk->setBlock(x % 16, y, block);
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

    TraceLog(LOG_INFO, "Begin deserializing chunks");

    for(int i = 0; i < chunkCount; i++) {
        int chunkSize = getBytes<unsigned int>();
        auto chunk = new Chunk(this);

        ByteVector chunkBytes(m_bytes.begin() + m_offset, m_bytes.begin() + m_offset + chunkSize);
        m_offset += chunk->deserialize(chunkBytes);

        m_chunks.push_back(chunk);
    }

    TraceLog(LOG_INFO, "End deserializing chunks");

    calcLightDepths();

    return m_offset;
}

void World::save() {
    auto worldData = this->serialize();
    if(SaveFileData("world.dat", worldData.data(), worldData.size())) {
        TraceLog(LOG_INFO, "Saved world to world.dat");
    }
}