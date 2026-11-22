#include "Stone.hpp"

std::unique_ptr<Item> Stone::dropItem(Item* tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_unique<Item>(TileID::COBBLESTONE);
    }

    return nullptr;
}