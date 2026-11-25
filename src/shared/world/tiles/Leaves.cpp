#include "Leaves.hpp"
#include <world/World.hpp>

void Leaves::onRandomTick(World& world, TilePosition pos) {
    Tile::onRandomTick(world, pos);

    bool isThereTree = false;

    for (int x = pos.x - 8; x < pos.x + 8; x++) {
        for (int y = pos.y - 8; y < pos.y + 8; y++) {
            auto tile0 = world.getTile({x, y, 0});
            auto tile1 = world.getTile({x, y, 1});

            isThereTree |= tile0 != TileID::AIR && tile0 == TileID::OAK_LOG;
            isThereTree |= tile1 != TileID::AIR && tile1 == TileID::OAK_LOG;
        }
    }

    if (!isThereTree) {
        world.destroyTile(pos);
    }
}