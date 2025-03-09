#pragma once
#include <world/blocks/Stone.hpp>

class GoldOre : public Stone {
public:
    GoldOre();

    std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool) override;
};