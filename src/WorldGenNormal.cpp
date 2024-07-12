#include <WorldGenNormal.hpp>
#include <World.hpp>
#include <Block.hpp>
#include <stdlib.h>

WorldGenNormal::WorldGenNormal(World *world) : WorldGen(world) {
    m_perlinNoise = GenImagePerlinNoise(world->getWidth(), 1, 0, 0, 2.0f);
}

WorldGenNormal::~WorldGenNormal() {
    UnloadImage(m_perlinNoise);
}

Block *WorldGenNormal::generateBlock(int x, int y) {
    Block::BlockType type;
    bool valid_block = true;

    int grassLevel = m_world->getHeight() * (*((unsigned char*)m_perlinNoise.data + (x * 4))) / 255;

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y > grassLevel && y < grassLevel + 5) {
        type = Block::BlockType::DIRT;
    } else if(y >= grassLevel + 5) {
        type = Block::BlockType::STONE;
    }else {
        valid_block = false;
    }

    if (!valid_block) return nullptr;

    auto ret = new Block(type);
    ret->setPosition(x, y);

    return ret;
}