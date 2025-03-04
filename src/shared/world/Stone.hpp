#pragma once
#include <world/Block.hpp>

class Stone : public Block {
public:
    Stone();

    std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool) override;
};