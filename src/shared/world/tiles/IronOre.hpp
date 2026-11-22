#pragma once
#include <world/tiles/Stone.hpp>

class IronOre : public Stone {
public:
    TileID getId() const override { return TileID::IRON_ORE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};