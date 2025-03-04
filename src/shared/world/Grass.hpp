#pragma once
#include <world/Block.hpp>

class Grass : public Block {
public:
    Grass();

    std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool) override;
};