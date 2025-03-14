#include <world/blocks/Grass.hpp>

Grass::Grass() : Block(GRASS) {}

std::shared_ptr<InventoryItem> Grass::dropItem(std::shared_ptr<Item> tool) {
    return std::make_shared<InventoryItem>(DIRT);
}