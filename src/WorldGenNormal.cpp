#include <WorldGenNormal.hpp>
#include <world.hpp>
#include <Block.hpp>
#include <stdlib.h>

WorldGenNormal::WorldGenNormal(World *world) : WorldGen(world) {}

Block *WorldGenNormal::generateBlock(int x, int y) {
    Block::BlockType type = Block::BlockType::AIR;
    bool valid_block = true;

    int grassLevel = (int)(m_world->m_height * 2 / 3);

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y > grassLevel && y < grassLevel + 5) {
        type = Block::BlockType::DIRT;
    } else if(y >= grassLevel + 5) {
        type = Block::BlockType::STONE;
    } else if(y == grassLevel - 1) {
        int value = rand() % 2;

        if (value == 0) {
            type = Block::BlockType::GRASS;
        } else {
            valid_block = false;
        }
    }
    
    else {
        valid_block = false;
    }

    if (!valid_block) return nullptr;

    auto ret = new Block(type);
    ret->setPosition(x, y);

    return ret;
}