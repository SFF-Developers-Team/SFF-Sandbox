#include "Chunk.hpp"
#include "ResourceManager.hpp"
#include "Vector2.hpp"
#include <raylib-cpp.hpp>
#include <World.hpp>

void World::Draw(raylib::Camera2D& camera, int width, int height) {
    auto startPos = camera.GetScreenToWorld({0, 0});
    auto endPos = camera.GetScreenToWorld({(float)width, (float)height});

    auto& rm = ResourceManager::Get();
    auto& blocksTilemap = rm.GetTilemap("blocks.png");
    
    if (startPos.x < 0.f) startPos.x -= 1.f;

    for (int x = startPos.GetX(); x < endPos.GetX(); x++) {
        for (int y = startPos.GetY(); y < endPos.GetY(); y++) {
            int chunkX = x / CHUNK_WIDTH;
            int chunkY = y / CHUNK_HEIGHT;

            if (m_chunks.contains({chunkX, chunkY})) {
                auto& chunk = m_chunks.at({chunkX, chunkY});
                auto blockId = chunk.GetBlock(x % CHUNK_WIDTH, y % CHUNK_HEIGHT);

                if (!blockId) {
                    continue;
                }
                
                blocksTilemap.DrawTile(blockId - 1, {(float)x, (float)y, 1.f, 1.f});
            }
        }
    }
}