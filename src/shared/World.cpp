#include "World.hpp"
#include "Chunk.hpp"
#include "Types.hpp"
#include "worldgen/Flat.hpp"
#include <algorithm>
#include <memory>
#include <vector>
#include "Entity.hpp"

World::World(std::string const& path) : m_worldGenerator(std::make_unique<Flat>(0)) {
    for (int x = -8; x < 8; x++) {
        for (int y = 0; y < 16; y++) {
            Chunk chunk;

            m_worldGenerator->GenerateChunk(chunk, {x, y});
            m_chunks[{x, y}] = chunk;
        }
    }
}

void World::Update() {
    for (auto& entity : m_entities) {
        entity->Update();
    }

    // std::erase_if(m_entities, [](auto& entity) -> bool {
    //     bool const sr = entity->ShouldRemove();
        
    //     if (sr) delete entity;

    //     return sr;
    // });
}

BlockID World::GetBlock(int x, int y) {
    const int chunkX = x / CHUNK_WIDTH;
    const int chunkY = y / CHUNK_HEIGHT;
    
    auto it = m_chunks.find({chunkX, chunkY});
    
    if (it != m_chunks.end()) {
        const int blockX = x % CHUNK_WIDTH;
        const int blockY = y % CHUNK_HEIGHT;
        
        return it->second.GetBlock(blockX, blockY);
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
            auto const block = GetBlock(x, y);

            if (block != BLOCK_ID_AIR) {
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