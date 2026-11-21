#pragma once
#include <world/blocks/Stone.hpp>

class GoldOre : public Stone {
public:
    BlockID getId() const override { return BlockID::GOLD_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};