#pragma once
#include <world/blocks/Stone.hpp>

class CoalOre : public Stone {
public:
    BlockID getId() const override { return BlockID::COAL_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};