#include <WorldGenRubyDung.hpp>
#include <World.hpp>
#include <Block.hpp>

WorldGenRubyDung::WorldGenRubyDung(World* world) : WorldGen(world) {}

Block *WorldGenRubyDung::generateBlock(int x, int y, uint8_t layer) {
    Block::BlockType type = Block::BlockType::AIR;

    int grassLevel = (int)(m_world->getHeight() * 2 / 3);

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y >= grassLevel) {
        type = Block::BlockType::STONE;
    } else {
        return nullptr;
    }

    auto ret = new Block(type);
    ret->setPosition(x, y, layer);

    return ret;
}