#include "Chunk.hpp"
#include "Utils.hpp"
#include <RenderTexture.hpp>
#include <algorithm>
#include <cstdint>
#include "World.hpp"

Chunk::Chunk() : m_dirtyLightning(true) {
    Fill(BLOCK_ID_AIR);
    std::fill(m_lightmap.begin(), m_lightmap.end(), 0xFF);
}

BlockID Chunk::GetBlock(int x, int y, int z) noexcept {
    if (x >= CHUNK_WIDTH || y >= CHUNK_HEIGHT) {
        return BLOCK_ID_AIR;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return BLOCK_ID_AIR;
    }

    if (x < 0)
        x += CHUNK_WIDTH;
    if (y < 0)
        y += CHUNK_HEIGHT;

    return m_blocks[CHUNK_INDEX(x, y, z)];
}

void Chunk::SetBlock(int x, int y, int z, BlockID blockId) {
    if (x >= CHUNK_WIDTH || y >= CHUNK_HEIGHT) {
        return;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return;
    }

    if (x < 0)
        x += CHUNK_WIDTH;
    if (y < 0)
        y += CHUNK_HEIGHT;

    m_blocks[CHUNK_INDEX(x, y, z)] = blockId;
    SetLightDirty();
}

void Chunk::Fill(BlockID blockId) {
    std::fill(m_blocks.begin(), m_blocks.end(), blockId);
}

void Chunk::SetLight(int x, int y, uint8_t level) {
    if (x >= CHUNK_WIDTH || y >= CHUNK_HEIGHT) {
        return;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return;
    }

    if (x < 0)
        x += CHUNK_WIDTH;
    if (y < 0)
        y += CHUNK_HEIGHT;

    m_lightmap[INDEX_2D(x, y, CHUNK_WIDTH)] = level;
}

uint8_t Chunk::GetLight(int x, int y) const noexcept {
    if (x >= CHUNK_WIDTH || y >= CHUNK_HEIGHT) {
        return 255;
    }

    if (x < -CHUNK_WIDTH || y < -CHUNK_HEIGHT) {
        return 255;
    }

    if (x < 0)
        x += CHUNK_WIDTH;
    if (y < 0)
        y += CHUNK_HEIGHT;

    return m_lightmap[INDEX_2D(x, y, CHUNK_WIDTH)];
}

#define MAX_LIGHT_Y 0 // above this height will only be light (chunk coord y)

void Chunk::UpdateLighting(World& world, const Vector2i& pos) {
    if (!IsLightDirty()) {
        return;
    }

    printf("Updating chunk light [%d, %d]\n", pos.x, pos.y);

    if (pos.y <= MAX_LIGHT_Y) {
        std::fill(m_lightmap.begin(), m_lightmap.end(), 0xFF);
        return;
    }

    Vector2i const upChunkCoord = {pos.x, pos.y - 1};
    Chunk* upChunk = world.GetChunk(upChunkCoord);
    if (!upChunk) {
        world.GenerateChunk(upChunkCoord);
        upChunk = world.GetChunk(upChunkCoord);
        if (!upChunk) {
            printf("failed to generate chunk above\n");
            return;
        }
        upChunk->SetLightDirty();
    }

    upChunk->UpdateLighting(world, upChunkCoord);

    uint8_t* lastRow = upChunk->m_lightmap.data() + (CHUNK_WIDTH * (CHUNK_HEIGHT - 1));

    bool thisLastRowChanged = false;
    for (int localX = 0; localX < CHUNK_WIDTH; ++localX) {
        int light = lastRow[localX];
        for (int localY = 0; localY < CHUNK_HEIGHT; ++localY) {
            uint8_t oldLight;
            if (!thisLastRowChanged && localY == CHUNK_HEIGHT - 1) {
                oldLight = m_lightmap[INDEX_2D(localX, localY, CHUNK_WIDTH)];
            }

            if (auto block = GetBlock(localX, localY, 1); block != BLOCK_ID_AIR && block != BLOCK_ID_ROSE) {
                SetLight(localX, localY, light);
                light = std::max(0, light - 50);
            } else {
                if (GetBlock(localX, localY, 0) != BLOCK_ID_AIR) {
                    SetLight(localX, localY, light);
                } else {
                    SetLight(localX, localY, 0xFF);
                }
            }

            if (!thisLastRowChanged && localY == CHUNK_HEIGHT - 1) {
                uint8_t newLight = m_lightmap[INDEX_2D(localX, localY, CHUNK_WIDTH)];
                thisLastRowChanged = newLight != oldLight;
            }
        }
    }

    SetLightDirty(false);

    if (thisLastRowChanged) {
        Vector2i const downChunkCoord = {pos.x, pos.y + 1};
        Chunk* downChunk = world.GetChunk(downChunkCoord);
        if (downChunk) {
            downChunk->SetLightDirty();
            downChunk->UpdateLighting(world, downChunkCoord);
        }
    }
}