#pragma once
#include <world/Block.hpp>

class Leaves : public Block {
public:
    Leaves();

    void onRandomTick(World* world, BlockPosition position) override;
    std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool) override;
};