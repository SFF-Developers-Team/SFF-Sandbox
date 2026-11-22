#pragma once
#include <world/Tile.hpp>

class Leaves : public Tile {
public:
    TileID getId() const override { return TileID::LEAVES; }
    void onRandomTick(World& world, TilePosition position) override;
    std::unique_ptr<Item> dropItem(Item* tool) override;
};