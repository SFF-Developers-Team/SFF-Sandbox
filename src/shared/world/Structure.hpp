#pragma once
#include <Types.hpp>

class Structure {
protected:
    BlockVector m_blocks;
    Vec2i m_size;

public:
    Structure(Vec2i size);

    Vec2i getSize() { return m_size; }
    
    int getIndex(BlockPosition pos) { return (pos.layer * m_size.x * m_size.y) + (pos.y * m_size.x) + abs(pos.x); }
    void setBlock(BlockPosition pos, std::shared_ptr<Block> block);
    std::shared_ptr<Block> getBlock(BlockPosition pos);
    bool isOutOfBound(BlockPosition pos);
};