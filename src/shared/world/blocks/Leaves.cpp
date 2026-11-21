#include <world/blocks/Leaves.hpp>
#include <world/World.hpp>

void Leaves::onRandomTick(World& world, BlockPosition pos) {
    Block::onRandomTick(world, pos);

    bool isThereTree = false;

    for (int x = pos.x - 4; x < pos.x + 4; x++) {
        for (int y = pos.y - 4; y < pos.y + 4; y++) {
            auto block0 = world.getBlock({x, y, 0});
            auto block1 = world.getBlock({x, y, 1});

            isThereTree |= block0 != BlockID::AIR && block0 == BlockID::OAK_LOG;
            isThereTree |= block1 != BlockID::AIR && block1 == BlockID::OAK_LOG;
        }
    }

    if (!isThereTree) {
        world.destroyBlock(pos);
    }
}

std::unique_ptr<Item> Leaves::dropItem(Item* tool) {
    return nullptr;
}