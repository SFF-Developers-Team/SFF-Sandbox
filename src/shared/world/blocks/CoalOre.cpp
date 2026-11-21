#include <world/blocks/CoalOre.hpp>

std::unique_ptr<Item> CoalOre::dropItem(Item* tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_unique<Item>(ItemID::COAL);
    }

    return nullptr;
}