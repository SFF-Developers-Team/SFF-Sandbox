#include "World.hpp"
#include "Blocks.hpp"
#include "Chunk.hpp"
#include "Types.hpp"
#include <algorithm>
#include <memory>
#include <vector>
#include "Entity.hpp"
#include "worldgen/WorldGenerator.hpp"

World::World(std::string const& path, std::unique_ptr<WorldGenerator> worldgen) : m_worldGenerator(std::move(worldgen)), m_ticks(10000) {
    for (int x = -8; x < 8; x++) {
        for (int y = 0; y < 16; y++) {
            Chunk chunk;

            m_worldGenerator->GenerateChunk(chunk, {x, y});
            m_chunks[{x, y}] = chunk;
        }
    }
}

void World::OnTick() {
    for (auto& entity : m_entities) {
        entity->OnTick();
    }

    m_ticks++;
}

BlockID World::GetBlock(int x, int y, int z) {
    const int chunkX = x / CHUNK_WIDTH;
    const int chunkY = y / CHUNK_HEIGHT;
    
    auto it = m_chunks.find({chunkX, chunkY});
    
    if (it != m_chunks.end()) {
        const int blockX = x % CHUNK_WIDTH;
        const int blockY = y % CHUNK_HEIGHT;
        
        return it->second.GetBlock(blockX, blockY, z);
    }

    return BLOCK_ID_AIR;
}

std::vector<Box> World::GetBlocksAround(Box const& box) {
    int minX = box.x - 1;
    int maxX = ceil(box.x + box.width);
    int minY = box.y - 1;
    int maxY = ceil(box.y + box.height);

    std::vector<Box> ret;

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            auto const block = GetBlock(x, y, 1);

            if (!gGhostBlocks.contains(block)) {
                ret.push_back(Box(x, y, 1.f, 1.f));
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

void World::BreakBlock(int x, int y, int z) {
    auto block = GetBlock(x, y, z);
    const int chunkX = x / CHUNK_WIDTH;
    const int chunkY = y / CHUNK_HEIGHT;
    
    if (block != BLOCK_ID_AIR && block != BLOCK_ID_ADMINIUM) {
        // Maybe we need to check gamemode of player to instant breaking block?
        m_chunks[{chunkX, chunkY}].SetBlock(x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z, BLOCK_ID_AIR);
    }
}
void World::SetBlock(int x, int y, int z, BlockID block) {
    auto blockInChunk = GetBlock(x, y, z);
    const int chunkX = x / CHUNK_WIDTH;
    const int chunkY = y / CHUNK_HEIGHT;
    
    m_chunks[{chunkX, chunkY}].SetBlock(x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z, block);
}

float const World::GetDaylightFactor() {
    int const ticksInDay = 24000;
    float const t = static_cast<float>(m_ticks % ticksInDay) / static_cast<float>(ticksInDay);
    float const phase = t * 2.0f * PI;  // Convert to radians

    return (std::sin(phase - PI/2) + 1.f) / 2.f;  // [0, 1]
}