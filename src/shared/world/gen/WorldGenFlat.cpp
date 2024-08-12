#include <WorldGenFlat.hpp>
#include <World.hpp>
#include <Block.hpp>

WorldGenFlat::WorldGenFlat(World* world) : WorldGen(world) {}

std::unique_ptr<Block> WorldGenFlat::generateBlock(int x, int y, uint8_t layer) {
    Block::BlockType type = Block::BlockType::AIR;
    int grassLevel = (int)(m_world->getHeight() * 2 / 3);

    if(y == m_world->getHeight() - 1) return std::make_unique<Block>(Block::BlockType::BEDROCK, x, y, layer);

    if(y == grassLevel) {
        type = Block::BlockType::GRASS;
    } else if(y > grassLevel && y < grassLevel + 5) {
        type = Block::BlockType::DIRT;
    } else if(y >= grassLevel + 5) {
        type = Block::BlockType::STONE;
    }

    return std::make_unique<Block>(type, x, y, layer);
}