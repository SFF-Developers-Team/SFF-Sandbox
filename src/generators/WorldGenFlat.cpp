#include <WorldGenFlat.hpp>
#include <World.hpp>
#include <Block.hpp>

WorldGenFlat::WorldGenFlat(World* world) : WorldGen(world) {}

Block *WorldGenFlat::generateBlock(int x, int y) {
    Block::BlockType type = Block::BlockType::AIR;
    bool valid_block = true;

    int grassLevel = (int)(m_world->getHeight() * 2 / 3);

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y > grassLevel && y < grassLevel + 5) {
        type = Block::BlockType::DIRT;
    } else if(y >= grassLevel + 5) {
        type = Block::BlockType::STONE;
    } else {
        return nullptr;
    }

    auto ret = new Block(type);
    ret->setPosition(x, y);

    return ret;
}