#pragma once
#include <world/tiles/Stone.hpp>

class DiamondOre : public Stone {
public:
    TileID getId() const override { return TileID::DIAMOND_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};