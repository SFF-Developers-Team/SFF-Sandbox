#include <world/Leaves.hpp>
#include <world/World.hpp>

Leaves::Leaves() : Block(ItemID::LEAVES) {
    setTag(TAG_COLOR, Col3u {0, 228, 48});
}

void Leaves::onRandomTick() {
    Block::onRandomTick();

    if (hasTag(TAG_NATURAL)) {
        bool isThereTree = false;

        for (int x = m_x - 4; x < m_x + 4; x++) {
            for (int y = m_y - 4; y < m_y + 4; y++) {
                auto block0 = m_world->getBlock(x, y, 0);
                auto block1 = m_world->getBlock(x, y, 1);

                isThereTree |= block0 != nullptr && block0->getID() == OAK_LOG;
                isThereTree |= block1 != nullptr && block1->getID() == OAK_LOG;
            }
        }

        if (!isThereTree) {
            m_world->setBlock(m_x, m_y, m_layer, nullptr);
        }
    }
}

std::shared_ptr<InventoryItem> Leaves::dropItem(std::shared_ptr<Item> tool) {
    return nullptr;
}