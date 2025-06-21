#include <world/blocks/CoalOre.hpp>

CoalOre::CoalOre() : Stone() {}

std::shared_ptr<InventoryItem> CoalOre::dropItem(std::shared_ptr<Item> tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_shared<InventoryItem>(COAL);
    }

    return nullptr;
}