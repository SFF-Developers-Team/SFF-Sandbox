#include <World.hpp>
#include <Player.hpp>
#include <cmath>
#include <Debug.hpp>
#include <WorldGenFlat.hpp>
#include <WorldGenNormal.hpp>
#include <Game.hpp>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_blocks.reserve(width * height);
    m_lightDepths = new float[width * height];
    m_player = new Player(this);

    generate();
    calcLightDepths();
}

World::~World() {
    delete[] m_lightDepths;
    delete m_player;
}

void World::generate() {
    WorldGenNormal generator = this;

    generator.generateWorld();
}

void World::update(int renderDistance) {
    auto playerPos = m_player->getPosition();
    m_playerInChunk = (int)(playerPos.x / BLOCK_SIZE_PIXELS / CHUNK_SIZE);
    m_renderMinX = fmax((m_playerInChunk - ceil(abs(renderDistance / 2))) * CHUNK_SIZE, 0);
    m_renderMaxX = fmin((m_playerInChunk + ceil(abs(renderDistance / 2))) * (CHUNK_SIZE * 2), m_width);
    m_hitboxes.clear();

    for (int x = (int)(playerPos.x / BLOCK_SIZE_PIXELS) - 5; x < (int)(playerPos.x / BLOCK_SIZE_PIXELS) + 5; x++) {
        for(int y = (int)(playerPos.y / BLOCK_SIZE_PIXELS) - 5; y < (int)(playerPos.y / BLOCK_SIZE_PIXELS) + 5; y++) {
            if(m_blocks[x * m_height + y] != nullptr && isBlockAccesible(x, y)) {
                m_hitboxes.push_back(Rectangle {(float)x * BLOCK_SIZE_PIXELS, (float)y * BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS});
            }
        }
    }

    m_player->update(m_hitboxes);

    Debug::addString(TextFormat("World size: %dx%d", m_width, m_height));
    Debug::addString(TextFormat("Render min X: %d", m_renderMinX));
    Debug::addString(TextFormat("Render max X: %d", m_renderMaxX));
    Debug::addString(TextFormat("Current chunk: %d", m_playerInChunk)); 
}

void World::draw(bool debug) {
    for (int x = m_renderMinX; x < m_renderMaxX; x++) {
        for(int y = 0; y < m_height; y++) {
            auto block = m_blocks[x * m_height + y];
            if(!block || block->getType() == Block::BlockType::AIR) continue;

            auto playerTarget = m_player->getTargetBlock();
            auto tilemap = Game::get()->getBlocksTileMap();
            auto dest = Rectangle {(float)x * BLOCK_SIZE_PIXELS, (float)y * BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS};
            Color color = ColorBrightness(WHITE, Clamp(m_lightDepths[x * m_height + y], -1.0f, 0.0f));

            // Самый жесткий костыль в мире
            if(playerTarget.x == x && playerTarget.y == y) {
                DrawRectangleRec(dest, WHITE);
                color = ColorAlpha(WHITE, sin(GetTime() * 10.f) * 0.2f + 0.4f);
            }

            tilemap->drawTilePro(tilemap->getPositionByIndex((uint8_t)block->getType() - 1), dest, color);
        }

        if(Debug::m_debug) {
            DrawLineV({
                (float)m_playerInChunk * CHUNK_SIZE * BLOCK_SIZE_PIXELS, 0}, 
                {(float)m_playerInChunk * CHUNK_SIZE * BLOCK_SIZE_PIXELS, (float)m_height * BLOCK_SIZE_PIXELS}, YELLOW
            );

            DrawLineV({
                (float)m_playerInChunk * (CHUNK_SIZE * 2) * BLOCK_SIZE_PIXELS, 0}, 
                {(float)m_playerInChunk * (CHUNK_SIZE * 2) * BLOCK_SIZE_PIXELS, (float)m_height * BLOCK_SIZE_PIXELS}, YELLOW
            );
        
            for(auto& hitbox : m_hitboxes) {
                DrawRectangleLinesEx(hitbox, 1.0f, RED);
            }
        }
    }

    m_player->draw();
}

bool World::isBlockAccesible(int x, int y) {
    if(x - 1 < -1 || y - 1 < -1 || x + 1 > m_width || y + 1 > m_height) return false;
    
    return m_blocks[x * m_height + (y + 1)] == nullptr || 
           m_blocks[x * m_height + (y - 1)] == nullptr || 
           m_blocks[(x + 1) * m_height + y] == nullptr || 
           m_blocks[(x - 1) * m_height + y] == nullptr;
}

void World::calcLightDepths() {
    std::fill_n(m_lightDepths, m_width * m_height, 0.0f);

    for(int x = 0; x < m_width; x++) {
        for(int y = 0; y < m_height; y++) {
            int i = x * m_height + y;
            int d = 1;
            while(m_lightDepths[i] > -1.0f) {
                if (m_blocks[(x - d) * m_height + y] != nullptr &&
                    m_blocks[(x + d) * m_height + y] != nullptr &&
                    m_blocks[x * m_height + (y - d)] != nullptr &&
                    m_blocks[x * m_height + (y + d)] != nullptr) {
                        d++;
                        m_lightDepths[i] -= 0.4f;
                } else {
                    break;
                }
            }
        }
    }
}

void World::placeBlock(int x, int y, enum Block::BlockType id) {
    int index = x * m_height + y;

    if(m_blocks[index] == nullptr){
        m_blocks[index] = new Block(id);
        calcLightDepths();
    }
}

void World::destroyBlock(int x, int y) {
    int index = x * m_height + y;

    if(m_blocks[index] != nullptr) {
        delete m_blocks[index];
        m_blocks[index] = nullptr;
        calcLightDepths();
    } 
}

Block *World::getBlock(int x, int y) {
    if (x > m_width || y < 0 || x < 0 || y > m_height) return nullptr;

    return m_blocks[x * m_height + y];
}

void World::setBlock(int x, int y, Block* block) {
    if(x < 0 || x > m_width || y < 0 || y > m_height) return;

    m_blocks[x * m_height + y] = block;
}