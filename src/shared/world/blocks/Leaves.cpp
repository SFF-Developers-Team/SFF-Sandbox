#include <world/blocks/Leaves.hpp>
#include <world/World.hpp>

Leaves::Leaves() : Block(ItemID::LEAVES) {
    setTag(TAG_COLOR, Col3u {0, 228, 48});
}

void Leaves::onRandomTick(World* world, BlockPosition position) {
    Block::onRandomTick(world, position);

    if (hasTag(TAG_NATURAL)) {
        bool isThereTree = false;

        for (int x = position.x - 4; x < position.x + 4; x++) {
            for (int y = position.y - 4; y < position.y + 4; y++) {
                auto block0 = world->getBlock(x, y, 0);
                auto block1 = world->getBlock(x, y, 1);

                isThereTree |= block0 != nullptr && block0->getID() == OAK_LOG;
                isThereTree |= block1 != nullptr && block1->getID() == OAK_LOG;
            }
        }

        if (!isThereTree) {
            world->setBlock(position.x, position.y, position.layer, nullptr);
        }
    }
}

std::shared_ptr<InventoryItem> Leaves::dropItem(std::shared_ptr<Item> tool) {
    return nullptr;
}