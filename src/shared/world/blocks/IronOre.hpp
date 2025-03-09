#pragma once
#include <world/blocks/Stone.hpp>

class IronOre : public Stone {
public:
    IronOre();

    std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool) override;
};