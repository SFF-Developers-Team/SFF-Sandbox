#include <World.hpp>
#include <Player.hpp>
#include <cmath>
#include <Debug.hpp>
#include <WorldGenFlat.hpp>
#include <WorldGenNormal.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <Human.hpp>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_player = new Player(this);
    m_header = WORLD;

    // for(int i = 0; i < 100; i++) {
    //     m_humans.push_back(new Human(this));
    // }

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

void World::update() {
    m_player->update();

    for(auto& human : m_humans) {
        human->update();
    }

    for(int i = 0; i < m_particles.capacity(); i++) {
        m_particles[i]->update();

        if(m_particles[i]->willRemove()) {
            m_particles.erase(m_particles.begin() + i);
        }
    }

    Debug::addString(TextFormat("World size: %dx%d", m_width, m_height));
}

void World::draw() {
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
            DrawLineV(
                {(float)chunk->getPosition() * CHUNK_SIZE * BLOCK_SIZE_PIXELS, 0}, 
                {(float)chunk->getPosition() * CHUNK_SIZE * BLOCK_SIZE_PIXELS, (float)m_height * BLOCK_SIZE_PIXELS}, YELLOW
            );

            DrawLineV(
                {(float)chunk->getPosition() * (CHUNK_SIZE * 2) * BLOCK_SIZE_PIXELS, 0}, 
                {(float)chunk->getPosition() * (CHUNK_SIZE * 2) * BLOCK_SIZE_PIXELS, (float)m_height * BLOCK_SIZE_PIXELS}, YELLOW
            );
        }
    }

    Debug::addString(TextFormat("Chunks drawn: %d", chunksDrawn));

    m_player->draw();

    for(auto& human : m_humans) {
        human->draw();
    }
}

bool World::isBlockClosed(int x, int y, uint8_t l) {
    return getBlock(x - 1, y, l) && getBlock(x + 1, y, l) && getBlock(x, y - 1, l) && getBlock(x, y + 1, l);
}

void World::calcLightDepths() {
    for(auto& chunk : m_chunks) {
        chunk->resetLightDepts();

        for(int x = 0; x < CHUNK_WIDTH; x++) {
            for(int y = 0; y < m_height; y++) {
                int d = 1;
                auto absX = chunk->getPosition() * CHUNK_WIDTH + x;

                while(chunk->getLightDepth(x, y) > -1.0f) {
                    if (getBlock(absX - d, y, 1) != nullptr && 
                        getBlock(absX + d, y, 1) != nullptr && 
                        getBlock(absX, y - d, 1) != nullptr && 
                        getBlock(absX, y + d, 1) != nullptr
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

void World::placeBlock(int x, int y, uint8_t layer, enum Block::BlockType id) {
    if(getBlock(x, y, layer) == nullptr){
        setBlock(x, y, layer, new Block(id));
        calcLightDepths();
    }
}

void World::destroyBlock(int x, int y, uint8_t layer) {
    auto block = getBlock(x, y, layer);
    auto game = Game::get();
    auto tilemap = game->getBlocksTileMap();

    if(!block) return;

    for(int offsetX = 0; offsetX < 8; offsetX++) {
        for(int offsetY = 0; offsetY < 8; offsetY++) {
            game->getParticleManager()->add(new Particle(this, 
            Vector2 {(float)x * BLOCK_SIZE_PIXELS + offsetX + 4, (float)y * BLOCK_SIZE_PIXELS + offsetY + 4}, 
            Vector2 {(rand() / RAND_MAX) * 50.f + 25.f * GetRandomValue(-1, 1), -75.f}, tilemap->getTextureOfTileCached((uint16_t)block->getType() - 1)));
        }
    }

    setBlock(x, y, layer, nullptr);
    calcLightDepths();
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

bool World::save() {
    auto worldData = this->serialize();
    return SaveFileData("world.dat", worldData.data(), worldData.size());
}

bool World::load() {
    if(!FileExists("world.dat")) return false;

    int worldSize;
    auto worldData = LoadFileData("world.dat", &worldSize);
        
    if(!worldSize) {
        UnloadFileData(worldData);
        return false;
    }

    auto worldBytes = ByteVector(worldData, worldData + worldSize);

    deserialize(worldBytes);
    UnloadFileData(worldData);
    
    return true;
}