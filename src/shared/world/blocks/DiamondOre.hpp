#pragma once
#include <world/blocks/Stone.hpp>

class DiamondOre : public Stone {
public:
    DiamondOre();

    std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool) override;
};