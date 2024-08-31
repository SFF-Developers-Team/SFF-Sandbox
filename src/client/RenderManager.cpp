#include "RenderManager.hpp"
#include "Debug.hpp"
#include <World.hpp>
#include <Player.hpp>
#include <Chunk.hpp>
#include <Game.hpp>
#include <Types.hpp>

RenderManager::RenderManager(World* world, std::shared_ptr<Player> player) : m_world(world), m_player(player) {
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

void RenderManager::renderChunk(std::shared_ptr<Chunk> chunk) {
    auto wh = m_world->getHeight();
    auto game = Game::get();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for(int y = 0; y < wh; y++) {
            auto block0 = chunk->getBlock(x, y, 0);
            auto block1 = chunk->getBlock(x, y, 1);

            // Пока что все блоки у нас одинакового размера
            if(!game->getPlayer()->isBlockInView(block0)) continue;
            auto tilemap = Game::get()->getBlocksTileMap();
            auto dest = Rectangle {
                // To support lightweight chunk format
                (chunk->getPosition() > 0 && x < CHUNK_SIZE ? (float)chunk->getPosition() * CHUNK_SIZE + x : x), 
                (float)y, 
                1.0f, 1.0f
            };

            if((block0 && block0->getType() != Block::Type::AIR) && (block1 && block1->getType() == Block::Type::AIR)) {
                tilemap->drawTilePro((uint16_t)block0->getType() - 1, dest, ColorBrightness(WHITE, -0.25f));
            }

            if(block1 && block1->getType() != Block::Type::AIR) {
                tilemap->drawTilePro((uint16_t)block1->getType() - 1, dest, WHITE);
            }
        }
    }
}

void RenderManager::renderEntity(Entity* entity) {
    DrawTexturePro(m_texture, {0, 0, (float)m_texture.width, (float)m_texture.height}, entity->getHitbox().getRect().to<Rectangle>(), {0, 0}, 0.0f, WHITE);
}

void RenderManager::renderSimplePlayer(std::shared_ptr<SimplePlayer> player) {
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
        auto size = MeasureTextEx(GetFontDefault(), username.c_str(), 0.5f, 0.05f);
        DrawTextEx(GetFontDefault(), username.c_str(), {hitbox.x + hitbox.width / 2.f - size.x / 2.f, hitbox.y - 1.f}, 0.5f, 0.05f, WHITE);
    }

    if(Debug::m_debug) {
        DrawRectangleLinesEx(hitbox.getRect().to<Rectangle>(), 0.025f, GREEN);
        DrawRectangleLinesEx(hitbox.expand(player->getSpeed().x, player->getSpeed().y).getRect().to<Rectangle>(), 0.05f, ORANGE);
    }
}