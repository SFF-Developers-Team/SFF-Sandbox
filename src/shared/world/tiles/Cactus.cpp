#include "Cactus.hpp"
#include <world/World.hpp>

void Cactus::tileChanged(World& world, TilePosition position) {
    if (world.getTile(position) == TileID::AIR) {
        position.y--;

        if (world.getTile(position) == TileID::CACTUS_HEAD) {
            world.destroyTile(position);
        }

        position.y += 2;

        if (world.getTile(position) == TileID::CACTUS_BASE) {
            world.destroyTile(position);
        }
    }
}