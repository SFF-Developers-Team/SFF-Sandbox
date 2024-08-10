#include "RenderManager.hpp"
#include "Debug.hpp"
#include <World.hpp>
#include <Player.hpp>
#include <Chunk.hpp>
#include <Game.hpp>
#include <Types.hpp>

RenderManager::RenderManager(World* world, Player* player) : m_world(world), m_player(player) {
    m_texture = LoadTexture("assets/player.png");
}

void RenderManager::renderWorld() {
    int chunksDrawn = 0;

    for (auto& chunk : m_world->getChunks()) {
        if(m_player->isChunkInView(chunk)) {
            renderChunk(chunk);
            chunksDrawn++;
        }

        auto playerTarget = m_player->getTargetBlock();

        // Selected block
        if(playerTarget.x > 0 && playerTarget.y > 0) {
            DrawRectangleLinesEx({playerTarget.x * BS, playerTarget.y * BS, BS, BS}, 1.0f, WHITE);
        }

        if(Debug::m_debug) {
            DrawLineV(
                {chunk->getPosition() * CHUNK_SIZE * BS, 0}, 
                {chunk->getPosition() * CHUNK_SIZE * BS, m_world->getHeight() * BS}, YELLOW
            );

            DrawLineV(
                {chunk->getPosition() * (CHUNK_SIZE * 2) * BS, 0}, 
                {chunk->getPosition() * (CHUNK_SIZE * 2) * BS, m_world->getHeight() * BS}, YELLOW
            );
        }
    }

    for(auto& [_, player] : m_world->getPlayers()) {
        renderSimplePlayer(player);
    }

    Debug::addString("World size: {}x{}", m_world->getWidth(), m_world->getHeight());
    Debug::addString("Chunks drawn: {}", chunksDrawn);
    Debug::addString("Player count: {}", m_world->getPlayers().size());
}

void RenderManager::renderChunk(Chunk* chunk) {
    auto wh = m_world->getHeight();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < wh; y++) {
            for(int layer = 0; layer < LAYERS; layer++) {
                auto block = chunk->getBlock(x, y, layer);
                if(!block || block->getType() == Block::BlockType::AIR) continue;

                auto tilemap = Game::get()->getBlocksTileMap();
                auto dest = Rectangle {
                    chunk->getPosition() * CHUNK_SIZE * BS + x * BS, 
                    y * BS, BS, BS
                };

                Color color = ColorBrightness(WHITE, (layer == 1) ? 1.0f : -0.25f);

                tilemap->drawTilePro((uint16_t)block->getType() - 1, dest, color);
            }
        }
    }
}

void RenderManager::renderEntity(Entity* entity) {
    DrawTexturePro(m_texture, {0, 0, (float)m_texture.width, (float)m_texture.height}, entity->getHitbox().to<Rectangle>(), {0, 0}, 0.0f, WHITE);
}

void RenderManager::renderSimplePlayer(SimplePlayer* player) {
    if(Debug::m_debug) {
        for(auto& hitbox : m_world->getHitboxes(player->getHitbox())) {
            DrawRectangleLinesEx(hitbox.to<Rectangle>(), 2.0f, RED);
        }
    }

    float frameWidth = m_texture.width / 17;
    auto hitbox = player->getHitbox();
    Rectangle src = {player->getAnimCurrentFrame() * frameWidth, 0, frameWidth * player->getDirection(), (float)m_texture.height};
    Rectangle dest = {hitbox.x + hitbox.width / 2 - frameWidth, hitbox.y + hitbox.height - m_texture.height * 2, frameWidth * 2, (float)m_texture.height * 2};
    DrawTexturePro(m_texture, src, dest, {0, 0}, 0, WHITE);

    auto username = player->getUsername();
    if(!username.empty()) {
        auto size = MeasureText(username.c_str(), 10);
        DrawText(username.c_str(), hitbox.x + hitbox.width / 2 - size / 2, hitbox.y - 15, 10, WHITE);
    }

    if(Debug::m_debug) {
        DrawRectangleLinesEx(hitbox.to<Rectangle>(), 1.0f, GREEN);
    }
}