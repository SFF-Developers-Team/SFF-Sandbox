#include "raylib.h"
#include <world/ClientChunk.hpp>
#include <managers/RenderManager.hpp>
#include <managers/Debug.hpp>
#include <managers/TextureManager.hpp>
#include <Utils.hpp>
#include <rlgl.h>

ClientChunk::ClientChunk() : Chunk() {}

ClientChunk::ClientChunk(Chunk&& chunk) 
    : Chunk(std::move(chunk)) { rebuild(); } 

void ClientChunk::rebuild() {
    std::vector<float> vertices;

    auto tilemap = TextureManager::get()->getTileMap("blocks.png");
    auto tex = tilemap->getMap();

    for (int y = 0; y < CHUNK_HEIGHT; y++) {
        for (int x = 0; x < CHUNK_WIDTH; x++) {
            auto block0 = getBlock({x, y, 0});
            auto block1 = getBlock({x, y, 0});

            float x0 = x;
            float y0 = y;
            float x1 = x + 1;
            float y1 = y + 1;

            if (block0 != nullptr && block1 == nullptr) {
                int tileX = block0->getSpriteID() % tex.width;
                int tileY = block0->getSpriteID() / tex.height;
                
                float uvSize = 1.0f / tex.width;
                float u0 = tileX * uvSize; 
                float v0 = tileY * uvSize;
                float u1 = u0 + uvSize;
                float v1 = v0 + uvSize;


                vertices.insert(vertices.end(), {
                    x0, y0, u0, v0,
                    x1, y0, u1, v0,
                    x1, y1, u1, v1,
    
                    x0, y0, u0, v0,
                    x1, y1, u1, v1,
                    x0, y1, u0, v1 
                });
            }
        }
    }

    m_vao = rlLoadVertexArray(); 
    rlEnableVertexArray(m_vao);

    m_vbo = rlLoadVertexBuffer(vertices.data(), vertices.size() * sizeof(float), false);
    rlSetVertexAttribute(0, 3, RL_FLOAT, false, 3 * sizeof(float), 0);
    
    rlDisableVertexArray();

    // for debug
    m_blockCount = countBlocks();
}

void ClientChunk::setBlock(BlockPosition pos, std::shared_ptr<Block> block) {
    Chunk::setBlock(pos, block);
    rebuild();
}

void ClientChunk::draw(Vec2i pos) {
    auto posf = pos.as<float>();


    auto tilemap = TextureManager::get()->getTileMap("blocks.png");
    auto tex = tilemap->getMap();

    rlPushMatrix();
    rlTranslatef(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, 0.0f);

    rlSetTexture(tex.id);
    rlEnableVertexArray(m_vao);
    rlDrawVertexArray(0, CHUNK_WIDTH * CHUNK_HEIGHT * 6);
    rlDisableVertexArray();
    rlSetTexture(0);

    rlPopMatrix();

    if (Debug::get()->isVisible()) {
        DrawRectangleLines(pos.x * CHUNK_WIDTH, pos.y * CHUNK_HEIGHT, CHUNK_WIDTH, CHUNK_HEIGHT, YELLOW);
    }
}