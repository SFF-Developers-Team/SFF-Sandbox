#pragma once
#include <world/Block.hpp>

class Leaves : public Block {
public:
    BlockID getId() const override { return BlockID::LEAVES; }
    void onRandomTick(World& world, BlockPosition position) override;
    std::unique_ptr<Item> dropItem(Item* tool) override;
};