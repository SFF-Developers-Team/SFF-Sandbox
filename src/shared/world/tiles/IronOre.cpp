#include "IronOre.hpp"

std::unique_ptr<Item> IronOre::dropItem(Item* tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_unique<Item>(ItemID::IRON_ORE);
    }

    return nullptr;
}