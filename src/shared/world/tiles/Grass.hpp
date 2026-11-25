#pragma once
#include <world/Tile.hpp>

class Grass : public Tile {
public:
    void onRandomTick(World& world, TilePosition position) override;
};