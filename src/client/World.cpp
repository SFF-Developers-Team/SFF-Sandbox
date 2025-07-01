#include "Blocks.hpp"
#include "Chunk.hpp"
#include "ResourceManager.hpp"
#include "Vector2.hpp"
#include <Color.hpp>
#include <algorithm>
#include <raylib-cpp.hpp>
#include <World.hpp>

void World::Draw(raylib::Camera2D& camera, int width, int height) {
    auto startPos = camera.GetScreenToWorld({0, 0});
    auto endPos = camera.GetScreenToWorld({(float)width, (float)height});

    auto& rm = ResourceManager::Get();
    auto& blocksTilemap = rm.GetTilemap("blocks.png");
    
    raylib::Color const backgroundBlock(0x7F7F7FFF);

    float const brightness = -0.8f + GetDaylightFactor();
    raylib::Color tint = raylib::Color::White().Brightness(brightness);

    if (startPos.x < 0.f) startPos.x -= 1.f;

    for (int x = startPos.GetX(); x < endPos.GetX(); x++) {
        for (int y = startPos.GetY(); y < endPos.GetY(); y++) {
            int chunkX = x / CHUNK_WIDTH;
            int chunkY = y / CHUNK_HEIGHT;

            if (m_chunks.contains({chunkX, chunkY})) {
                auto& chunk = m_chunks.at({chunkX, chunkY});

                auto blockId0 = chunk.GetBlock(x % CHUNK_WIDTH, y % CHUNK_HEIGHT, 0);
                auto blockId1 = chunk.GetBlock(x % CHUNK_WIDTH, y % CHUNK_HEIGHT, 1);

                bool isBlock1Transperent = gTransperentBlocks.contains(blockId1);

                if ((!blockId1 || isBlock1Transperent) && blockId0) {
                    blocksTilemap.DrawTile(blockId0 - 1, {(float)x, (float)y, 1.f, 1.f}, tint.Brightness(-0.5f));
                    continue;
                }
                
                if (blockId1) {
                    blocksTilemap.DrawTile(blockId1 - 1, {(float)x, (float)y, 1.f, 1.f}, tint);
                }
            }
        }
    }
}