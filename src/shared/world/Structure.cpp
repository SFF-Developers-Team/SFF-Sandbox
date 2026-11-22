#include <Types.hpp>
#include <world/Structure.hpp>

Structure::Structure(Vec2i pos, Vec2i size) : m_pos(pos), m_size(size), m_tiles(size.x * size.y * 2) {}

bool Structure::isOutOfBound(TilePosition pos) {
    return (
        pos.x < 0 || pos.x > m_size.x || 
        pos.y < 0 || pos.y > m_size.y || 
        pos.layer < 0 || pos.layer > 1
    );
}

void Structure::setTile(TilePosition pos, std::shared_ptr<Tile> tile) {
    if (!isOutOfBound(pos)) {
        m_tiles[getIndex(pos)] = tile;
    }
}

std::shared_ptr<Tile> Structure::getTile(TilePosition pos) {
    if (!isOutOfBound(pos)) {
        return m_tiles[getIndex(pos)];
    }

    return nullptr;
}