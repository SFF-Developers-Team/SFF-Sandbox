#include <world/Structure.hpp>

Structure::Structure(Vec2i size) : m_size(size) {}


bool Structure::isOutOfBound(BlockPosition pos) {
    return (
        pos.x < 0 || pos.x > m_size.x || 
        pos.y < 0 || pos.y > m_size.y || pos.layer < 0 || 
        pos.layer > 1
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