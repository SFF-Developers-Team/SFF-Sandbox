#include <Chunk.hpp>
#include <Block.hpp>
#include <World.hpp>
#include <stdlib.h>
#include <WorldGenNormal.hpp>

WorldGenNormal::WorldGenNormal(World* world) : WorldGen(world), m_perlinNoise(1) {}

WorldGenNormal::~WorldGenNormal() {}

std::unique_ptr<Block> WorldGenNormal::generateBlock(int x, int y, uint8_t layer) {
    Block::BlockType type = Block::BlockType::AIR;
    int grassLevel = m_world->getHeight() * m_perlinNoise.noise2D_01(x * 0.01f, layer * 0.01f);

    if(y == m_world->getHeight() - 1) return std::make_unique<Block>(Block::BlockType::BEDROCK, x, y, layer);
    if(y >= grassLevel + 10 && layer == 1 && round(m_perlinNoise.noise2D_01(x * 0.2f, y * 0.2f)) == 1) return std::make_unique<Block>(Block::BlockType::AIR, x, y, layer);;

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y > grassLevel && y < grassLevel + 5) {
        type = Block::BlockType::DIRT;
    } else if(y >= grassLevel + 5) {
        type = Block::BlockType::STONE;
    }

    return std::make_unique<Block>(type, x, y, layer);
}