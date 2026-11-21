#pragma once
#include <world/Block.hpp>

class Grass : public Block {
public:
    BlockID getId() const override { return BlockID::GRASS; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};