#pragma once
#include <world/blocks/Stone.hpp>

class DiamondOre : public Stone {
public:
    BlockID getId() const override { return BlockID::DIAMOND_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};