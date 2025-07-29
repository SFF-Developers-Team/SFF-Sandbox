#include "Blocks.hpp"
#include "Chunk.hpp"
#include "ResourceManager.hpp"
#include "Vector2.hpp"
#include <Camera2D.hpp>
#include <Color.hpp>
#include <Rectangle.hpp>
#include <RenderTexture.hpp>
#include <cstdint>
#include <raylib-cpp.hpp>
#include <World.hpp>
#include <raylib.h>

// TODO: Remove repeat code

void World::Draw(raylib::Camera2D& camera, int renderWidth, int renderHeight) {
    auto const startPos = camera.GetScreenToWorld({0, 0});
    auto const endPos = camera.GetScreenToWorld({(float)renderWidth, (float)renderHeight});

    auto& rm = ResourceManager::Get();
    auto& blocksTilemap = rm.GetTilemap("blocks.png");
    raylib::Color const backgroundBlock(0x7F7F7FFF);

    float const brightness = -0.8f + GetDaylightFactor();
    raylib::Color baseTint = raylib::Color::White().Brightness(brightness);
    raylib::Color const darkerTint = baseTint.Brightness(-0.5f);

    int const startX = static_cast<int>(std::floor(startPos.x));
    int const startY = static_cast<int>(std::floor(startPos.y));
    int const endX = static_cast<int>(std::ceil(endPos.x));
    int const endY = static_cast<int>(std::ceil(endPos.y));

    int const startChunkX = (startX < 0) ? startX / CHUNK_WIDTH - 1 : startX / CHUNK_WIDTH;
    int const startChunkY = (startY < 0) ? startY / CHUNK_HEIGHT - 1 : startY / CHUNK_HEIGHT;
    int const endChunkX = endX / CHUNK_WIDTH;
    int const endChunkY = endY / CHUNK_HEIGHT;

    for (int chunkY = startChunkY; chunkY <= endChunkY; chunkY++) {
        for (int chunkX = startChunkX; chunkX <= endChunkX; chunkX++) {
            auto const chunkPos = Vector2i {chunkX, chunkY};
            auto const it = m_chunks.find(chunkPos);

            if (it == m_chunks.end()) {
                continue;
            }

            auto& chunk = it->second;

            int const chunkStartX = std::max(startX, chunkX * CHUNK_WIDTH);
            int const chunkStartY = std::max(startY, chunkY * CHUNK_HEIGHT);
            int const chunkEndX = std::min(endX, (chunkX + 1) * CHUNK_WIDTH);
            int const chunkEndY = std::min(endY, (chunkY + 1) * CHUNK_HEIGHT);

            for (int y = chunkStartY; y < chunkEndY; ++y) {
                int const localY = y % CHUNK_HEIGHT;

                for (int x = chunkStartX; x < chunkEndX; ++x) {
                    int const localX = x % CHUNK_WIDTH;

                    auto const blockId0 = chunk.GetBlock(localX, localY, 0);
                    auto const blockId1 = chunk.GetBlock(localX, localY, 1);

                    if (!blockId0 && !blockId1) continue;

                    bool const isBlock1Transparent = blockId1 ? gTransperentBlocks.contains(blockId1) : false;

                    if (blockId0 && (!blockId1 || isBlock1Transparent)) {
                        blocksTilemap.DrawTile(blockId0 - 1, {(float)x, (float)y, 1.f, 1.f}, darkerTint);
                    }

                    if (blockId1) {
                        blocksTilemap.DrawTile(blockId1 - 1, {(float)x, (float)y, 1.f, 1.f}, baseTint);
                    }
                }
            }
        }
    }
}

void World::PrepareLightmap(raylib::Camera2D& camera, raylib::RenderTexture2D& lightmap, int renderWidth, int renderHeight) {
    auto const startPos = camera.GetScreenToWorld({0, 0});
    auto const endPos = camera.GetScreenToWorld({(float)renderWidth, (float)renderHeight});

    int const startX = static_cast<int>(std::floor(startPos.x));
    int const startY = static_cast<int>(std::floor(startPos.y));
    int const endX = static_cast<int>(std::ceil(endPos.x));
    int const endY = static_cast<int>(std::ceil(endPos.y));

    int const startChunkX = (startX < 0) ? startX / CHUNK_WIDTH - 1 : startX / CHUNK_WIDTH;
    int const startChunkY = (startY < 0) ? startY / CHUNK_HEIGHT - 1 : startY / CHUNK_HEIGHT;
    int const endChunkX = endX / CHUNK_WIDTH;
    int const endChunkY = endY / CHUNK_HEIGHT;

    lightmap.BeginMode();
    ClearBackground(BLANK);

    for (int chunkY = startChunkY; chunkY <= endChunkY; chunkY++) {
        for (int chunkX = startChunkX; chunkX <= endChunkX; chunkX++) {
            auto const chunkPos = Vector2i {chunkX, chunkY};
            auto const it = m_chunks.find(chunkPos);

            if (it == m_chunks.end()) {
                continue;
            }

            auto& chunk = it->second;

            int const chunkStartX = std::max(startX, chunkX * CHUNK_WIDTH);
            int const chunkStartY = std::max(startY, chunkY * CHUNK_HEIGHT);
            int const chunkEndX = std::min(endX, (chunkX + 1) * CHUNK_WIDTH);
            int const chunkEndY = std::min(endY, (chunkY + 1) * CHUNK_HEIGHT);

            for (int y = chunkStartY; y < chunkEndY; ++y) {
                int const localY = y % CHUNK_HEIGHT;

                for (int x = chunkStartX; x < chunkEndX; ++x) {
                    int const localX = x % CHUNK_WIDTH;

                    const auto blockPos = camera.GetWorldToScreen({(float)x, (float)y});
                    const auto blockSize = camera.GetWorldToScreen({x + 1.f, y + 1.f}) - blockPos;
                    raylib::Rectangle(blockPos, blockSize).Draw({0, 0, 0, (uint8_t)(255 - chunk.GetLight(localX, localY))});
                }
            }
        }
    }

    lightmap.EndMode();
}