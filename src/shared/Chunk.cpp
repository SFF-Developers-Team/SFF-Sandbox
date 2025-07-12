#include "Chunk.hpp"
#include "Utils.hpp"
#include <RenderTexture.hpp>
#include <algorithm>
#include <cstdint>

Chunk::Chunk() : m_dirtyLightning(true) {
    Fill(BLOCK_ID_AIR);
    std::fill(m_lightmap.begin(), m_lightmap.end(), 0xFF);
}

BlockID Chunk::GetBlock(int x, int y, int z) noexcept {
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
    SetLightDirty();
}

void Chunk::Fill(BlockID blockId) {
    std::fill(m_blocks.begin(), m_blocks.end(), blockId);
}

void Chunk::SetLight(int x, int y, uint8_t level) {
    if (x > CHUNK_WIDTH || y > CHUNK_HEIGHT) {
        return;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return;
    }

    if (x < 0) x += CHUNK_WIDTH;
    if (y < 0) y += CHUNK_HEIGHT;

    m_lightmap[INDEX_2D(x, y, CHUNK_WIDTH)] = level;
}

uint8_t Chunk::GetLight(int x, int y) const noexcept {
    if (x > CHUNK_WIDTH || y > CHUNK_HEIGHT) {
        return 255;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return 255;
    }

    if (x < 0) x += CHUNK_WIDTH;
    if (y < 0) y += CHUNK_HEIGHT;

    return m_lightmap[INDEX_2D(x, y, CHUNK_WIDTH)];
}