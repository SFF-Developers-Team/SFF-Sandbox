#include <world.hpp>
#include <iostream>
#include <Sprite.hpp>
#include <cmath>

World::World(int width, int height) : m_width(width), m_height(height) {
    m_blocks = new Block[width * height];

    this->generate();
}

void World::generate() {
    std::fill_n(m_blocks, m_width * m_height, AIR);

    for (int x = 0; x < m_width; x++) {
        for(int y = 0; y < m_height; y++){
            int index = x * m_height + y;

            // m_blocks[index] = STONE;

            int grassLevel = (int)(m_height * 2 / 3);

            if(y == grassLevel) {
                m_blocks[index] = GRASS;
                continue;
            }

            if(y > grassLevel && y < grassLevel + 5) {
                m_blocks[index] = DIRT;
                continue;
            }

            if(y >= grassLevel + 5) {
                m_blocks[index] = STONE;
                continue;
            }
        }
    }
}

void World::update(Vector2 playerPosition, int renderDistance) {
    m_playerInChunk = (int)(playerPosition.x / 32 / CHUNK_SIZE) % CHUNK_SIZE;

    int minX = fmax((m_playerInChunk - ceil(renderDistance / 2)) * CHUNK_SIZE, 0);
    int maxX = fmin((m_playerInChunk + ceil(renderDistance / 2)) * CHUNK_SIZE, m_width);

    if(m_renderMinX != minX || m_renderMaxX != maxX) {
        this->buildHitboxes();
    }

    m_renderMinX = minX;
    m_renderMaxX = maxX;
}

void World::buildHitboxes() {
    m_hitboxes.clear();

    for (int x = m_renderMinX; x < m_renderMaxX; x++) {
        for(int y = 0; y < m_height; y++) {
            if(isBlockAccesible(x, y) && m_blocks[x * m_height + y] != AIR) {
                m_hitboxes.push_back(Vector2 {(float)x, (float)y});
            }
        }
    }
}

void World::draw(bool debug) {
    for (int x = m_renderMinX; x < m_renderMaxX; x++) {
        for(int y = 0; y < m_height; y++) {
            auto block = m_blocks[x * m_height + y];
            auto sprite = Sprite::getSprite(block);

            if(block == AIR) continue;

            DrawTexturePro(sprite->getTexture(), {0, 0, (float)sprite->getWidth(), (float)sprite->getHeight()}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE);
        }

        if(debug) {
            DrawLineV({(float)m_playerInChunk * CHUNK_SIZE * 32, 0}, {(float)m_playerInChunk * CHUNK_SIZE * 32, (float)m_height * 32}, YELLOW);
            DrawLineV({(float)m_playerInChunk * (CHUNK_SIZE * 2) * 32, 0}, {(float)m_playerInChunk * (CHUNK_SIZE * 2) * 32, (float)m_height * 32}, YELLOW);
        
            for(auto& hitbox : m_hitboxes) {
                DrawRectangleLinesEx(Rectangle {hitbox.x * 32, hitbox.y * 32, 32, 32}, 1.0f, RED);
            }
        }
    }
}

bool World::isBlockAccesible(int x, int y) {
    return m_blocks[x * m_height + (y + 1)] == AIR || 
           m_blocks[x * m_height + (y - 1)] == AIR || 
           m_blocks[(x + 1) * m_height + y] == AIR || 
           m_blocks[(x - 1) * m_height + y] == AIR;
}
