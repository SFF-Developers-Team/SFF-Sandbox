#pragma once
#include <world/blocks/Stone.hpp>

class IronOre : public Stone {
public:
    BlockID getId() const override { return BlockID::IRON_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};