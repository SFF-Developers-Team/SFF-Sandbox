#include "raylib.h"
#include <world/ClientChunk.hpp>
#include <managers/RenderManager.hpp>
#include <managers/Debug.hpp>
#include <Utils.hpp>

ClientChunk::ClientChunk() : Chunk(), m_render(LoadRenderTexture(16 * CHUNK_WIDTH, 16 * CHUNK_HEIGHT)) {}

ClientChunk::ClientChunk(Chunk&& chunk) 
    : Chunk(std::move(chunk)), m_render(LoadRenderTexture(16 * CHUNK_WIDTH, 16 * CHUNK_HEIGHT)) {} 

ClientChunk::~ClientChunk() {
    UnloadRenderTexture(m_render);
}

void ClientChunk::updateRender() {
    BeginTextureMode(m_render);
    ClearBackground({0, 0, 0, 0});
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            auto block0 = getBlock({x, y, 0});
            auto block1 = getBlock({x, y, 1});
            Rectf dest = {static_cast<float>(x * 16), static_cast<float>(y * 16), 16, 16};

            if (block0 != nullptr && block1 == nullptr) {
                RenderManager::renderBlock(dest, {x, y, 0}, block0);
            }

            if (block1 != nullptr) {
                RenderManager::renderBlock(dest, {x, y, 1}, block1);
            }
        }
    }
    EndTextureMode();

    // for debug
    m_blockCount = countBlocks();
}

void ClientChunk::setBlock(BlockPosition pos, std::shared_ptr<Block> block) {
    Chunk::setBlock(pos, block);
    updateRender();
}

void ClientChunk::draw(Vec2i pos) {
    auto posf = pos.to<Vec2f>();

    DrawTexturePro(m_render.texture, 
        {0.f, 0.f, 16.f * CHUNK_WIDTH, -16.f * CHUNK_HEIGHT}, 
        {posf.x * CHUNK_WIDTH, posf.y * CHUNK_HEIGHT, CHUNK_WIDTH, CHUNK_HEIGHT}, 
        {0.f, 0.f}, 0.f, WHITE
    );

    if (Debug::get()->isVisible()) {
        DrawRectangleLines(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, CHUNK_WIDTH, CHUNK_HEIGHT, YELLOW);
    }
}