#include "World.hpp"
#include "Blocks.hpp"
#include "Chunk.hpp"
#include "Types.hpp"
#include <ShaderUnmanaged.hpp>
#include <algorithm>
#include <cstdio>
#include <memory>
#include <raylib.h>
#include <vector>
#include "Entity.hpp"
#include "worldgen/WorldGenerator.hpp"

World::World(std::string const& path, std::unique_ptr<WorldGenerator> worldgen) : m_worldGenerator(std::move(worldgen)), m_ticks(10000) {
    for (int x = -8; x < 8; x++) {
        for (int y = 0; y < 16; y++) {
            Vector2i const pos(x, y);
            Chunk chunk {};

            m_worldGenerator->GenerateChunk(chunk, pos);
            AddChunk(pos, std::move(chunk));
        }
    }
}

void World::OnTick() {
    for (auto& entity : m_entities) {
        entity->OnTick();
    }

    UpdateLightning();

    m_ticks++;
}

BlockID World::GetBlock(int x, int y, int z) {
    int const chunkX = (x >= 0) ? x / CHUNK_WIDTH : (x - CHUNK_WIDTH + 1) / CHUNK_WIDTH;
    int const chunkY = (y >= 0) ? y / CHUNK_HEIGHT : (y - CHUNK_HEIGHT + 1) / CHUNK_HEIGHT;

    auto const it = m_chunks.find({chunkX, chunkY});
    if (it != m_chunks.end()) {
        int const blockX = ((x % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;
        int const blockY = ((y % CHUNK_HEIGHT) + CHUNK_HEIGHT) % CHUNK_HEIGHT;

        return it->second.GetBlock(blockX, blockY, z);
    }

    return BLOCK_ID_AIR;
}

std::vector<Box> World::GetBlocksAround(Box const& box) {
    int const minX = static_cast<int>(std::floor(box.x)) - 1;
    int const maxX = static_cast<int>(std::ceil(box.x + box.width));
    int const minY = static_cast<int>(std::floor(box.y)) - 1;
    int const maxY = static_cast<int>(std::ceil(box.y + box.height));

    auto getChunkCoord = [](int worldCoord, int chunkSize) { return worldCoord >= 0 ? worldCoord / chunkSize : (worldCoord + 1) / chunkSize - 1; };

    int const chunkMinX = getChunkCoord(minX, CHUNK_WIDTH);
    int const chunkMaxX = getChunkCoord(maxX, CHUNK_WIDTH);
    int const chunkMinY = getChunkCoord(minY, CHUNK_HEIGHT);
    int const chunkMaxY = getChunkCoord(maxY, CHUNK_HEIGHT);

    std::vector<Box> ret;
    ret.reserve((maxX - minX + 1) * (maxY - minY + 1) / 2);

    for (int chunkY = chunkMinY; chunkY <= chunkMaxY; ++chunkY) {
        for (int chunkX = chunkMinX; chunkX <= chunkMaxX; ++chunkX) {
            auto const it = m_chunks.find({chunkX, chunkY});
            if (it == m_chunks.end()) continue;

            auto& chunk = it->second;

            int const chunkWorldX = chunkX * CHUNK_WIDTH;
            int const chunkWorldY = chunkY * CHUNK_HEIGHT;

            int const localMinX = std::max(minX - chunkWorldX, 0);
            int const localMaxX = std::min(maxX - chunkWorldX, CHUNK_WIDTH - 1);
            int const localMinY = std::max(minY - chunkWorldY, 0);
            int const localMaxY = std::min(maxY - chunkWorldY, CHUNK_HEIGHT - 1);

            for (int localY = localMinY; localY <= localMaxY; localY++) {
                for (int localX = localMinX; localX <= localMaxX; localX++) {
                    BlockID id = chunk.GetBlock(localX, localY, 1);
                    
                    if (!gGhostBlocks.contains(id)) {
                        ret.emplace_back(chunkWorldX + localX, chunkWorldY + localY, 1.f, 1.f);
                    }
                }
            }
        }
    }

    return ret;
}

void World::AddEntity(class Entity* ptr) {
    if (std::find(m_entities.begin(), m_entities.end(), ptr) != m_entities.end()) {
        return;
    }

    m_entities.push_back(ptr);
}

void World::SetBlock(int x, int y, int z, BlockID block) {
    int const chunkX = (x >= 0) ? x / CHUNK_WIDTH : (x - CHUNK_WIDTH + 1) / CHUNK_WIDTH;
    int const chunkY = (y >= 0) ? y / CHUNK_HEIGHT : (y - CHUNK_HEIGHT + 1) / CHUNK_HEIGHT;

    int const blockX = ((x % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;
    int const blockY = ((y % CHUNK_HEIGHT) + CHUNK_HEIGHT) % CHUNK_HEIGHT;

    m_chunks[{chunkX, chunkY}].SetBlock(blockX, blockY, z, block);
}

float const World::GetDaylightFactor() {
    int const ticksInDay = 24000;
    float const t = static_cast<float>(m_ticks % ticksInDay) / static_cast<float>(ticksInDay);
    float const phase = t * 2.0f * PI; // Convert to radians

    return (std::sin(phase - PI / 2) + 1.f) / 2.f; // [0, 1]
}

void World::AddChunk(Vector2i pos, Chunk&& chunk) {
    m_chunks.emplace(pos, std::move(chunk));
}

void World::UpdateLightning() {
    for (auto& [position, chunk] : m_chunks) {
        if (!chunk.IsLightDirty()) {
            continue;
        }

        printf("Updating chunk light [%d, %d]\n", position.x, position.y);

        for (int localX = 0; localX < CHUNK_WIDTH; ++localX) {
            int worldX = position.x * CHUNK_WIDTH + localX;

            int light = 255;
            for (int localY = 0; localY < CHUNK_HEIGHT; ++localY) {
                int worldY = position.y * CHUNK_WIDTH + localY;

                // TODO
            }
        }

        chunk.SetLightDirty(false);
    }
}