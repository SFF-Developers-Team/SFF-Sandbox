#pragma once
#include <Types.hpp>

class Structure {
protected:
    TileVector m_tiles;
    Vec2i m_pos;
    Vec2i m_size;

public:
    Structure(Vec2i pos, Vec2i size);

    Vec2i getPos() { return m_pos; }
    Vec2i getSize() { return m_size; }
    
    int getIndex(TilePosition pos) { return (pos.layer * m_size.x * m_size.y) + (pos.y * m_size.x) + pos.x; }
    void setTile(TilePosition pos, std::shared_ptr<Tile> tile);
    std::shared_ptr<Tile> getTile(TilePosition pos);
    bool isOutOfBound(TilePosition pos);
};