#pragma once
#include <world/Tile.hpp>

class Stone : public Tile {
public:
    TileID getId() const override { return TileID::STONE; }
    std::unique_ptr<Item> dropItem(Item* tool) override;
};