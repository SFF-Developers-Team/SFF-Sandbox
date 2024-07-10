#include <World.hpp>
#include <iostream>
#include <Sprite.hpp>
#include <cmath>
#include "Debug.hpp"

World::World(int width, int height) : m_width(width), m_height(height) {
    m_blocks.reserve(width * height);

    this->generate();
}

void World::generate() {
    for (int x = 0; x < m_width; x++) {
        for(int y = 0; y < m_height; y++){
            int index = x * m_height + y;

            m_blocks[index] = nullptr;

            int grassLevel = (int)(m_height * 2 / 3);

            if(y == grassLevel) {
                m_blocks[index] = new Block(Block::BlockType::GRASS);
                continue;
            }

            if(y > grassLevel && y < grassLevel + 5) {
                m_blocks[index] = new Block(Block::BlockType::DIRT);
                continue;
            }

            if(y >= grassLevel + 5) {
                m_blocks[index] = new Block(Block::BlockType::STONE);
                continue;
            }
        }
    }
}

void World::update(Vector2 playerPosition, int renderDistance) {
    m_playerInChunk = (int)(playerPosition.x / BLOCK_SIZE_PIXELS / CHUNK_SIZE);
    m_renderMinX = fmax((m_playerInChunk - ceil(abs(renderDistance / 2))) * CHUNK_SIZE, 0);
    m_renderMaxX = fmin((m_playerInChunk + ceil(abs(renderDistance / 2))) * (CHUNK_SIZE * 2), m_width);
    m_hitboxes.clear();

    for (int x = (int)(playerPosition.x / BLOCK_SIZE_PIXELS) - 5; x < (int)(playerPosition.x / BLOCK_SIZE_PIXELS) + 5; x++) {
        for(int y = (int)(playerPosition.y / BLOCK_SIZE_PIXELS) - 5; y < (int)(playerPosition.y / BLOCK_SIZE_PIXELS) + 5; y++) {
            if(m_blocks[x * m_height + y] != nullptr && isBlockAccesible(x, y)) {
                m_hitboxes.push_back(Rectangle {(float)x * BLOCK_SIZE_PIXELS, (float)y * BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS, (float)BLOCK_SIZE_PIXELS});
            }
        }
    }

    Debug::addString(TextFormat("World size: %dx%d", m_width, m_height));
    Debug::addString(TextFormat("Render min X: %d", m_renderMinX));
    Debug::addString(TextFormat("Render max X: %d", m_renderMaxX));
    Debug::addString(TextFormat("Current chunk: %d", m_playerInChunk)); 
}

void World::draw(bool debug) {
    for (int x = m_renderMinX; x < m_renderMaxX; x++) {
        for(int y = 0; y < m_height; y++) {
            auto block = m_blocks[x * m_height + y];
            if(!block) continue;

            auto sprite = block->getSprite();

            DrawTexturePro(
                sprite->getTexture(), 
                {0, 0, (float)sprite->getWidth(), (float)sprite->getHeight()}, 
                {(float)x * BLOCK_SIZE_PIXELS, (float)y * BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS}, 
                {0, 0}, 0, WHITE
            );
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
}

bool World::isBlockAccesible(int x, int y) {
    if(x + 1 > m_width || y + 1 > m_height) return false;
    
    return m_blocks[x * m_height + (y + 1)] == nullptr || 
           m_blocks[x * m_height + (y - 1)] == nullptr || 
           m_blocks[(x + 1) * m_height + y] == nullptr || 
           m_blocks[(x - 1) * m_height + y] == nullptr;
}

void World::placeBlock(int x, int y, enum Block::BlockType id) {
    int index = x * m_height + y;

    if (m_blocks[index] != nullptr) delete m_blocks[index];
    m_blocks[index] = new Block(id);
}
void World::destroyBlock(int x, int y) {
    int index = x * m_height + y;

    if (m_blocks[index] != nullptr) delete m_blocks[index];
}