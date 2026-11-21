#include <world/blocks/DiamondOre.hpp>

std::unique_ptr<Item> DiamondOre::dropItem(Item* tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_unique<Item>(ItemID::DIAMOND);
    }

    return nullptr;
}