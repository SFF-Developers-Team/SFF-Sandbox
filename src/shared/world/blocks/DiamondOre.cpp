#include <world/blocks/DiamondOre.hpp>

DiamondOre::DiamondOre() : Stone() {}

std::shared_ptr<InventoryItem> DiamondOre::dropItem(std::shared_ptr<Item> tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_shared<InventoryItem>(DIAMOND);
    }

    return nullptr;
}