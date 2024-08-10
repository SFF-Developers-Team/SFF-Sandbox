#include <WorldGenRubyDung.hpp>
#include <World.hpp>
#include <Block.hpp>

WorldGenRubyDung::WorldGenRubyDung(World* world) : WorldGen(world) {}

std::unique_ptr<Block> WorldGenRubyDung::generateBlock(int x, int y, uint8_t layer) {
    Block::BlockType type = Block::BlockType::AIR;

    int grassLevel = (int)(m_world->getHeight() * 2 / 3);

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y >= grassLevel) {
        type = Block::BlockType::STONE;
    } else {
        type = Block::BlockType::AIR;
    }

    return std::make_unique<Block>(type, x, y, layer);;
}