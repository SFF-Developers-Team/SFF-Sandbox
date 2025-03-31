#include <Types.hpp>
#include <world/Structure.hpp>

Structure::Structure(Vec2i pos, Vec2i size) : m_pos(pos), m_size(size), m_blocks(size.x * size.y * 2) {}

bool Structure::isOutOfBound(BlockPosition pos) {
    return (
        pos.x < 0 || pos.x > m_size.x || 
        pos.y < 0 || pos.y > m_size.y || 
        pos.layer < 0 || pos.layer > 1
    );
}

void Structure::setBlock(BlockPosition pos, std::shared_ptr<Block> block) {
    if (!isOutOfBound(pos)) {
        m_blocks[getIndex(pos)] = block;
    }
}

std::shared_ptr<Block> Structure::getBlock(BlockPosition pos) {
    if (!isOutOfBound(pos)) {
        return m_blocks[getIndex(pos)];
    }

    return nullptr;
}