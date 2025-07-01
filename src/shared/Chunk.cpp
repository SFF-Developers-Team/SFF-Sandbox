#include "Chunk.hpp"
#include "Types.hpp"
#include <algorithm>

Chunk::Chunk() {
    Fill(BLOCK_ID_AIR);
}

BlockID Chunk::GetBlock(int x, int y, int z) {
    if (x > CHUNK_WIDTH || y > CHUNK_HEIGHT) {
        return BLOCK_ID_AIR;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return BLOCK_ID_AIR;
    }

    if (x < 0) x += CHUNK_WIDTH;
    if (y < 0) y += CHUNK_HEIGHT;

    return m_blocks[CHUNK_INDEX(x, y, z)];
}

void Chunk::SetBlock(int x, int y, int z, BlockID blockId) {
    if (x > CHUNK_WIDTH || y > CHUNK_HEIGHT) {
        return;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return;
    }

    if (x < 0) x += CHUNK_WIDTH;
    if (y < 0) y += CHUNK_HEIGHT;

    m_blocks[CHUNK_INDEX(x, y, z)] = blockId;
}

void Chunk::Fill(BlockID blockId) {
    std::fill(m_blocks.begin(), m_blocks.end(), blockId);
}