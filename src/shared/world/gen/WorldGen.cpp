#include <world/gen/WorldGen.hpp>
#include <world/World.hpp>
#include <stdio.h>

WorldGen::WorldGen(std::shared_ptr<World> world, uint64_t seed) : m_world(world), m_seed(seed) {}

void WorldGen::generateBlock(std::shared_ptr<Chunk> chunk, BlockPosition pos, ItemID id) {
    auto block = Block::create(id);
    block->setTag(TAG_NATURAL, true);

    chunk->setBlock(pos, block);
}