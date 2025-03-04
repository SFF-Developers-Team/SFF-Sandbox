#include <world/Stone.hpp>

Stone::Stone() : Block(STONE) {
    m_materialType = MATERIAL_STONE;
}

std::shared_ptr<InventoryItem> Stone::dropItem(std::shared_ptr<Item> tool) {
    if (tool && tool->getType() == TYPE_TOOL_PICKAXE) {
        return std::make_shared<InventoryItem>(COBBLESTONE);
    }

    return nullptr;
}