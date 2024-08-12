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
            DrawRectangleLinesEx({(float)playerTarget.x, (float)playerTarget.y, 1.0f, 1.0f}, 0.025f, WHITE);
        }

        if(Debug::m_debug) {
            DrawLineV(
                {(float)chunk->getPosition() * CHUNK_SIZE, 0}, 
                {(float)chunk->getPosition() * CHUNK_SIZE, (float)m_world->getHeight()}, YELLOW
            );

            DrawLineV(
                {(float)chunk->getPosition() * (CHUNK_SIZE * 2), 0}, 
                {(float)chunk->getPosition() * (CHUNK_SIZE * 2), (float)m_world->getHeight()}, YELLOW
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
                auto dest = Rectangle {(float)chunk->getPosition() * CHUNK_SIZE + x, (float)y, 1.0f, 1.0f};

                Color color = ColorBrightness(WHITE, (layer == 1) ? 1.0f : -0.25f);

                tilemap->drawTilePro((uint16_t)block->getType() - 1, dest, color);
            }
        }
    }
}

void RenderManager::renderEntity(Entity* entity) {
    DrawTexturePro(m_texture, {0, 0, (float)m_texture.width, (float)m_texture.height}, entity->getHitbox().getRect().to<Rectangle>(), {0, 0}, 0.0f, WHITE);
}

void RenderManager::renderSimplePlayer(SimplePlayer* player) {
    if(Debug::m_debug) {
        for(auto& hitbox : m_world->getHitboxes(player->getHitbox())) {
            DrawRectangleLinesEx(hitbox.getRect().to<Rectangle>(), 0.05f, RED);
        }
    }

    float frameWidth = m_texture.width / 17;
    auto hitbox = player->getHitbox();
    Rectangle src = {player->getAnimCurrentFrame() * frameWidth, 0, frameWidth * (player->getDirection() == Entity::Direction::LEFT ? 1.0f : -1.0f), (float)m_texture.height};
    Rectangle dest = {
        hitbox.x + (hitbox.width / 2) - (hitbox.width * 1.45f) / 2, 
        hitbox.y, 
        hitbox.width * 1.45f, 
        hitbox.height
    };
    DrawTexturePro(m_texture, src, dest, {0, 0}, 0, WHITE);

    auto username = player->getUsername();
    if(!username.empty()) {
        auto size = MeasureText(username.c_str(), 10);
        DrawText(username.c_str(), hitbox.x + hitbox.width / 2 - size / 2, hitbox.y - 15, 10, WHITE);
    }

    if(Debug::m_debug) {
        DrawRectangleLinesEx(hitbox.getRect().to<Rectangle>(), 0.025f, GREEN);
        DrawRectangleLinesEx(hitbox.expand(player->getSpeed().x, player->getSpeed().y).getRect().to<Rectangle>(), 0.05f, ORANGE);
    }
}