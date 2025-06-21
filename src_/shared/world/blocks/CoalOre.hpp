#pragma once
#include <world/blocks/Stone.hpp>

class CoalOre : public Stone {
public:
    CoalOre();

    std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool) override;
};