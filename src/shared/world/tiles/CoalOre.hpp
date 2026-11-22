#pragma once
#include <world/tiles/Stone.hpp>

class CoalOre : public Stone {
public:
    TileID getId() const override { return TileID::COAL_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};