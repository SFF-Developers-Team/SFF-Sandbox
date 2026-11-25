#pragma once
#include <world/Tile.hpp>

class Leaves : public Tile {
public:
    void onRandomTick(World& world, TilePosition position) override;
};