#include "Chunk.hpp"
#include <World.hpp>
#include <Player.hpp>
#include <Game.hpp>

Chunk::Chunk(World* world, int pos) {
    m_world = world;
    m_position = pos;
    m_blocks.reserve(CHUNK_WIDTH * world->getHeight());
    m_lightDepths = new float[CHUNK_WIDTH * world->getHeight()];
}

Chunk::~Chunk() {
    delete[] m_lightDepths;
}

bool Chunk::isOutOfBound(int x, int y) {
    return (x < 0 || x > CHUNK_WIDTH || y < 0 || y > m_world->getHeight());
}

void Chunk::setBlock(int x, int y, Block* block) {
    if(isOutOfBound(x, y)) return;
    
    // // Destroy block
    // if(block == nullptr && getBlock(x, y) != nullptr) {
    //     delete m_blocks[x * m_world->getHeight() + y];        
    // }

    m_blocks[x * m_world->getHeight() + y] = block;
}

void Chunk::setBlock(int x, int y, Block::BlockType type) {
    setBlock(x, y, new Block(type));
}

void Chunk::setBlock(Vector2 pos, Block::BlockType type) {
    setBlock(pos.x, pos.y, type);
}

Block* Chunk::getBlock(int x, int y) {
    if(isOutOfBound(x, y)) return nullptr;

    return m_blocks[x * m_world->getHeight() + y];
}

Block* Chunk::getBlock(Vector2 pos) {
    return getBlock(pos.x, pos.y);
}

bool Chunk::isBlockClosed(int x, int y) {
    return getBlock(x - 1, y) && getBlock(x + 1, y) && getBlock(x, y - 1) && getBlock(x, y + 1);
}

bool Chunk::isBlockClosed(Vector2 pos) {
    return isBlockClosed(pos.x, pos.y);
}

void Chunk::generate(WorldGen* gen) {
    for(int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < m_world->getHeight(); y++) {
            setBlock(x, y, gen->generateBlock(m_position * CHUNK_SIZE + x, y));
        }
    }
}

void Chunk::resetLightDepts() {
    std::fill_n(m_lightDepths, CHUNK_WIDTH * m_world->getHeight(), 0.0f);
}

void Chunk::setLightDepth(int x, int y, float d) {
    if(isOutOfBound(x, y)) return;

    m_lightDepths[x * m_world->getHeight() + y] = Clamp(d, -1.0f, 1.0f);
}

float Chunk::getLightDepth(int x, int y) {
    if(isOutOfBound(x, y)) return 0.0f;

    return m_lightDepths[x * m_world->getHeight() + y];
}

void Chunk::draw() {
    auto wh = m_world->getHeight();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < wh; y++) {
            auto block = m_blocks[x * wh + y];
            if(!block || block->getType() == Block::BlockType::AIR) continue;

            auto tilemap = Game::get()->getBlocksTileMap();
            auto dest = Rectangle {
                (float)(m_position * CHUNK_SIZE * BLOCK_SIZE_PIXELS + x * BLOCK_SIZE_PIXELS), 
                (float)(y * BLOCK_SIZE_PIXELS), 
                BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS
            };

            Color color = ColorBrightness(WHITE, getLightDepth(x, y));

            tilemap->drawTilePro(tilemap->getPositionByIndex((uint8_t)block->getType() - 1), dest, color);
        }
    }
}