#include <world/blocks/GoldOre.hpp>

GoldOre::GoldOre() : Stone() {}

std::shared_ptr<InventoryItem> GoldOre::dropItem(std::shared_ptr<Item> tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_shared<InventoryItem>(GOLD_ORE);
    }

    return nullptr;
}