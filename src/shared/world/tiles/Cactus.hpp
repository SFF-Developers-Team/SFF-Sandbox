#pragma once
#include <world/Tile.hpp>

class Cactus : public Tile {
public:
    void tileChanged(World& world, TilePosition position) override;
};