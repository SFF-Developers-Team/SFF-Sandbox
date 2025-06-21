#include <world/blocks/IronOre.hpp>

IronOre::IronOre() : Stone() {}

std::shared_ptr<InventoryItem> IronOre::dropItem(std::shared_ptr<Item> tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_shared<InventoryItem>(IRON_ORE);
    }

    return nullptr;
}