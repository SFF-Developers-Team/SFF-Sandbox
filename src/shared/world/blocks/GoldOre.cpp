#include <world/blocks/GoldOre.hpp>

std::unique_ptr<Item> GoldOre::dropItem(Item* tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_unique<Item>(ItemID::GOLD_ORE);
    }

    return nullptr;
}