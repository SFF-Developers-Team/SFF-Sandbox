#include <world/Grass.hpp>

Grass::Grass() : Block(GRASS) {
    m_materialType = MATERIAL_DIRT;
}

std::shared_ptr<InventoryItem> Grass::dropItem(std::shared_ptr<Item> tool) {
    return std::make_shared<InventoryItem>(DIRT);
}