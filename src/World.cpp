#include <World.hpp>
#include <Player.hpp>
#include <cmath>
#include <Debug.hpp>
#include <WorldGenFlat.hpp>
#include <WorldGenNormal.hpp>
#include <Game.hpp>
#include <Chunk.hpp>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_player = new Player(this);

    generate();
    calcLightDepths();
}

World::~World() {
    delete m_player;
}

void World::generate() {
    for(int x = 0; x < m_width / CHUNK_SIZE; x++) {
        auto chunk = new Chunk(this, x);
        chunk->generate(new WorldGenNormal(this));

        m_chunks.push_back(chunk);
    }
}

void World::update(int renderDistance) {
    auto pp = m_player->getPosition();
    m_hitboxes.clear();

    for (int x = (int)(pp.x / BLOCK_SIZE_PIXELS) - 5; x < (int)(pp.x / BLOCK_SIZE_PIXELS) + 5; x++) {
        for(int y = (int)(pp.y / BLOCK_SIZE_PIXELS) - 5; y < (int)(pp.y / BLOCK_SIZE_PIXELS) + 5; y++) {
            if(getBlock(x, y) != nullptr && !isBlockClosed(x, y)) {
                m_hitboxes.push_back(Rectangle {(float)x * BLOCK_SIZE_PIXELS, (float)y * BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS});
            }
        }
    }

    m_player->update(m_hitboxes);

    Debug::addString(TextFormat("World size: %dx%d", m_width, m_height));
}

void World::draw(bool debug) {
    for (auto& chunk : m_chunks) {
        chunk->draw();

        auto playerTarget = m_player->getTargetBlock();

        // Selected block
        if(playerTarget.x > 0 && playerTarget.y > 0) {
            DrawRectangleV(Vector2Multiply(playerTarget, {BS, BS}), {BS, BS}, ColorAlpha(WHITE, sin(GetTime() * 10.f) * 0.2f + 0.4f));
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

    m_player->draw();
}

bool World::isBlockClosed(int x, int y) {
    auto chunk = getChunk(x / CHUNK_SIZE);
    if(!chunk) return true;
    
    return chunk->isBlockClosed(x % CHUNK_SIZE, y);
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