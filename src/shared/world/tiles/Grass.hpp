#pragma once
#include <world/Tile.hpp>

class Grass : public Tile {
public:
    TileID getId() const override { return TileID::GRASS; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};