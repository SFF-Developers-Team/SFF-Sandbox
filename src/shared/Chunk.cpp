#include "Chunk.hpp"
#include "Utils.hpp"
#include <RenderTexture.hpp>
#include <algorithm>
#include <cstdint>
#include <queue>
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
#define LIGHT_DECAY 30
#define LIGHT_SOLID_DECAY 30

void Chunk::UpdateLighting(World& world, const Vector2i& pos) {
    if (!IsLightDirty()) {
        return;
    }

    // printf("Updating chunk light [%d, %d]\na", pos.x, pos.y);

    world.chunkLightUpdates++;

    if (pos.y <= MAX_LIGHT_Y) {
        std::fill(m_lightmap.begin(), m_lightmap.end(), 0xFF);
        return;
    }
    // std::fill(m_lightmap.begin(), m_lightmap.end(), 0x00);

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

    // flood fill
    struct _light_struct {
        int x;
        int y;
        uint8_t strength;
    };

    std::deque<_light_struct> queue;

    bool thisLastRowChanged = false;
    for (int localX = 0; localX < CHUNK_WIDTH; ++localX) {
        uint8_t light = lastRow[localX];
        for (int localY = 0; localY < CHUNK_HEIGHT; ++localY) {
            uint8_t oldLight;
            if (!thisLastRowChanged && localY == CHUNK_HEIGHT - 1) {
                oldLight = m_lightmap[INDEX_2D(localX, localY, CHUNK_WIDTH)];
            }

            auto block = GetBlock(localX, localY, 1);
            auto bg = GetBlock(localX, localY, 0);

            if (block == BLOCK_ID_AIR || block == BLOCK_ID_ROSE) { // if block is transparent
                queue.emplace_back(_light_struct {localX, localY, light});
            } else { // if block is solid
                light = 0;
            }

            SetLight(localX, localY, light);

            if (!thisLastRowChanged && localY == CHUNK_HEIGHT - 1) {
                uint8_t newLight = m_lightmap[INDEX_2D(localX, localY, CHUNK_WIDTH)];
                thisLastRowChanged = newLight != oldLight;
            }
        }
    }

    SetLightDirty(false);

    // if (thisLastRowChanged) {
    Vector2i const downChunkCoord = {pos.x, pos.y + 1};
    Chunk* downChunk = world.GetChunk(downChunkCoord);
    if (downChunk) {
        downChunk->SetLightDirty();
        downChunk->UpdateLighting(world, downChunkCoord);
    }
    // }

    auto leftChunk = world.GetChunk({pos.x - 1, pos.y});
    if (leftChunk) {
        leftChunk->UpdateLighting(world, {pos.x - 1, pos.y});
    }
    auto rightChunk = world.GetChunk({pos.x + 1, pos.y});
    if (rightChunk) {
        rightChunk->UpdateLighting(world, {pos.x + 1, pos.y});
    }

    while (queue.size() > 0) {
        auto l = queue.front();
        queue.pop_front();

        Vector2i newPos[4] = {
            {l.x - 1, l.y},
            {l.x + 1, l.y},
            {l.x, l.y - 1},
            {l.x, l.y + 1},
        };

        uint8_t newLight = std::max(0, l.strength - LIGHT_DECAY);

        if (newLight > 0) {
            for (auto& pos : newPos) {
                // if (pos.x < 0 || pos.y < 0 || pos.x >= CHUNK_WIDTH || pos.y >= CHUNK_HEIGHT)
                //     continue;
                int localX = pos.x;
                int localY = pos.y;
                Chunk* curChunk = this;
                // this doesnt really work =(
                if (pos.x < 0) {
                    if (leftChunk) {
                        curChunk = leftChunk;
                        localX += CHUNK_WIDTH;
                    } else {
                        continue;
                    }
                }
                if (pos.x >= CHUNK_WIDTH) {
                    if (rightChunk) {
                        curChunk = rightChunk;
                        localX -= CHUNK_WIDTH;
                    } else {
                        continue;
                    }
                }
                if (pos.y < 0 || pos.y >= CHUNK_HEIGHT)
                    continue;
                auto block = curChunk->GetBlock(localX, localY, 1);
                auto bg = curChunk->GetBlock(localX, localY, 0);

                auto curNewLight = newLight;
                if (block != BLOCK_ID_AIR && block != BLOCK_ID_ROSE) {
                    curNewLight = std::max(0, curNewLight - LIGHT_SOLID_DECAY);
                    if (curNewLight == 0)
                        continue;
                }
                uint8_t& addr = curChunk->m_lightmap[INDEX_2D(localX, localY, CHUNK_WIDTH)];
                if (curNewLight > addr) {
                    addr = curNewLight;
                    queue.emplace_back(_light_struct {pos.x, pos.y, curNewLight});
                }
            }
        }
    }
}