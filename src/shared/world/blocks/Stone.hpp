#pragma once
#include <world/Block.hpp>

class Stone : public Block {
public:
    BlockID getId() const override { return BlockID::STONE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};