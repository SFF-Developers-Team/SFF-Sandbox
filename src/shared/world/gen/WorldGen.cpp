#include <world/gen/WorldGen.hpp>
#include <world/World.hpp>

WorldGen::WorldGen(uint64_t seed) : m_seed(seed) {}

std::shared_ptr<Chunk> WorldGen::generateChunk(World* world, Vec2i position) {
    return std::make_shared<Chunk>();
}

void WorldGen::generateBlock(std::shared_ptr<Chunk> chunk, BlockPosition pos, ItemID id) {
    auto block = Block::create(id);
    block->setTag(TAG_NATURAL, true);

    chunk->setBlock(pos, block);
}