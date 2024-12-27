#include "RenderManager.hpp"
#include "Debug.hpp"
#include <World.hpp>
#include <Player.hpp>
#include <Chunk.hpp>
#include <Game.hpp>
#include <Types.hpp>
#include <ColoredBlock.hpp>

RenderManager::RenderManager(World* world, std::shared_ptr<Player> player) : m_world(world), m_player(player) {
    m_texture = LoadTexture("assets/player.png");
}

void RenderManager::renderWorld() {
    int chunksDrawn = 0;

    for (auto& [pos, chunk] : m_world->getChunks()) {
        if(m_player->isChunkInView(chunk)) {
            renderChunk(chunk);
            chunksDrawn++;
        }

        auto playerTarget = m_player->getTargetBlock();

        // Selected block
        DrawRectangleLinesEx({(float)playerTarget.x, (float)playerTarget.y, 1.0f, 1.0f}, 0.025f, WHITE);

        if(Debug::m_debug) {
            DrawLineV(
                {(float)pos * CHUNK_WIDTH, 0}, 
                {(float)pos * CHUNK_WIDTH, (float)m_world->getHeight()}, YELLOW
            );

            DrawLineV(
                {(float)pos * CHUNK_WIDTH + CHUNK_WIDTH, 0}, 
                {(float)pos * CHUNK_WIDTH + CHUNK_WIDTH, (float)m_world->getHeight()}, YELLOW
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

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for(int y = 0; y < wh; y++) {
            auto block0 = chunk->getBlock(x, y, 0);
            auto block1 = chunk->getBlock(x, y, 1);

            // Пока что все блоки у нас одинакового размера
            if(!game->getPlayer()->isBlockInView(block0)) continue;
            auto tilemap = Game::get()->getBlocksTileMap();
            auto blockX = ((chunk->getPosition() > 0 || chunk->getPosition() < 0) && x < CHUNK_WIDTH ? (float)chunk->getPosition() * CHUNK_WIDTH + x : x);

            if((block0 && block0->getID() != Block::ID::AIR) && (block1 && block1->getID() == Block::ID::AIR)) {
                renderBlock(blockX, y, block0);
            }

            if(block1 && block1->getID() != Block::ID::AIR) {
                renderBlock(blockX, y, block1);
            }
        }
    }
}

void RenderManager::renderBlock(float x, float y, std::shared_ptr<Block> block) {
    if(!block || block->getID() == Block::ID::AIR) {
        return;
    }

    auto tilemap = Game::get()->getBlocksTileMap();
    auto color = (block->getType() == Block::Type::COLORED ? std::dynamic_pointer_cast<ColoredBlock>(block)->getColor().to<Color>() : WHITE);
    auto dest = Rectangle {x, y, 1.0f, 1.0f};
    
    tilemap->drawTilePro((uint16_t)block->getID() - 1, dest, ColorBrightness(color, (block->getLayer() == 0 ? -0.25f : 1.0f)));
}

void RenderManager::renderSelectedBlock(float x, float y, std::shared_ptr<Block> block) {
    if(!block || block->getID() == Block::ID::AIR) {
        return;
    }

    auto tilemap = Game::get()->getBlocksTileMap();
    auto color = (block->getType() == Block::Type::COLORED ? std::dynamic_pointer_cast<ColoredBlock>(block)->getColor().to<Color>() : WHITE);
    color.a = 255;
    auto dest = Rectangle {x, y, 32.0f, 32.0f};
    
    tilemap->drawTilePro((uint16_t)block->getID() - 1, dest, color);
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
    }
}