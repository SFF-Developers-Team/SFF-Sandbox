#include <world/blocks/Leaves.hpp>
#include <world/World.hpp>

Leaves::Leaves() : Block(ItemID::LEAVES) {
    setTag(TAG_COLOR, Col3u {0, 228, 48});
}

void Leaves::onRandomTick(World* world, BlockPosition pos) {
    Block::onRandomTick(world, pos);

    if (hasTag(TAG_NATURAL)) {
        bool isThereTree = false;

        for (int x = pos.x - 4; x < pos.x + 4; x++) {
            for (int y = pos.y - 4; y < pos.y + 4; y++) {
                auto block0 = world->getBlock({x, y, 0});
                auto block1 = world->getBlock({x, y, 1});

                isThereTree |= block0 != nullptr && block0->getID() == OAK_LOG;
                isThereTree |= block1 != nullptr && block1->getID() == OAK_LOG;
            }
        }

        if (!isThereTree) {
            world->setBlock(pos, nullptr);
        }
    }
}

std::shared_ptr<InventoryItem> Leaves::dropItem(std::shared_ptr<Item> tool) {
    return nullptr;
}