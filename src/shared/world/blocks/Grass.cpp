#include <world/blocks/Grass.hpp>

std::unique_ptr<Item> Grass::dropItem(Item* tool) {
    return std::make_unique<Item>(BlockID::DIRT);
}