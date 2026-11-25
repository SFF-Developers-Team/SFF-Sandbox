#include "Grass.hpp"
#include <world/World.hpp>

void Grass::onRandomTick(World& world, TilePosition position) {
    position.y--;
    auto tile = world.getTile(position);
    position.y++;
    
    if (tile != TileID::AIR && !Tile::IsPhantom(tile)) {
        world.setTile(position, TileID::DIRT);
    }

    bool canGrowLeft = world.getTile({position.x - 1, position.y, position.layer}) == TileID::DIRT &&
                       world.getTile({position.x - 1, position.y - 1, position.layer}) == TileID::AIR;
    
    bool canGrowRight = world.getTile({position.x + 1, position.y, position.layer}) == TileID::DIRT &&
                        world.getTile({position.x + 1, position.y - 1, position.layer}) == TileID::AIR;
    
    if (canGrowLeft) {
        world.setTile({position.x - 1, position.y, position.layer}, TileID::GRASS);
    }

    if (canGrowRight) {
        world.setTile({position.x + 1, position.y, position.layer}, TileID::GRASS);
    }
}