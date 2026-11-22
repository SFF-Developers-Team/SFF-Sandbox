#pragma once
#include <world/tiles/Stone.hpp>

class GoldOre : public Stone {
public:
    TileID getId() const override { return TileID::GOLD_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};