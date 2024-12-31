#include "RenderManager.hpp"
#include "Debug.hpp"
#include <World.hpp>
#include <Player.hpp>
#include <Chunk.hpp>
#include <Game.hpp>
#include <Types.hpp>
#include <TextureManager.hpp>
#include <string>

int cdi;

RenderManager::RenderManager(std::shared_ptr<World> world, std::shared_ptr<Player> player) : m_world(world), m_player(player) {
    // Debug::addString("World size: {}x{}", m_world->getWidth(), m_world->getHeight());
    // Debug::addString("Chunks drawn: {}", chunksDrawn);
    // Debug::addString("Player count: {}", m_world->getPlayers().size());
    auto dbg = Debug::get();
    dbg->addString("World size: {}x{}", m_world->getWidth(), m_world->getHeight());
    cdi = dbg->addString("Chunks rendered: ");
}

void RenderManager::renderWorld() {
    auto dbg = Debug::get();
    int chunksCount = 0;

    for (auto& [pos, chunk] : m_world->getChunks()) {
        if(m_player->isChunkInView(chunk)) {
            renderChunk(chunk);
            chunksCount++;
        }

        auto layer = IsKeyDown(KEY_LEFT_ALT);
        auto target = m_player->getTargetBlock();

        // Selected block
        if(m_player->canAccessBlock(target, layer)) {
            DrawRectangleLinesEx({(float)target.x, (float)target.y, 1.0f, 1.0f}, 0.025f, WHITE);
        }

        if(dbg->isVisible()) {
            DrawLineV(
                {(float)pos * CHUNK_WIDTH, 0}, 
                {(float)pos * CHUNK_WIDTH, (float)m_world->getHeight()}, YELLOW
            );

            DrawLineV(
                {(float)pos * CHUNK_WIDTH + CHUNK_WIDTH, 0}, 
                {(float)pos * CHUNK_WIDTH + CHUNK_WIDTH, (float)m_world->getHeight()}, YELLOW
            );

            dbg->updateString(cdi, "Chunks rendered: {}", chunksCount);
        }
    }

    for(auto& [_, player] : m_world->getPlayers()) {
        renderSimplePlayer(player);
    }
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
    auto dest = Rectangle {x, y, 1.0f, 1.0f};
    Color col = (block->hasTag(Block::TagID::COLOR) ? block->getTag<Col3u>(Block::TagID::COLOR).to<Color>() : WHITE);
    col.a = 255;

    auto t = m_player->getTargetBlock();
    
    tilemap->drawTilePro((uint16_t)block->getID() - 1, dest, ColorBrightness(col, (block->getLayer() == 0 ? -0.25f : 0.0f)));
}

void RenderManager::renderSelectedBlock(float x, float y, std::shared_ptr<Block> block) {
    if(!block || block->getID() == Block::ID::AIR) {
        return;
    }

    auto tilemap = Game::get()->getBlocksTileMap();
    auto dest = Rectangle {x, y, 32.0f, 32.0f};
    Color col = (block->hasTag(Block::TagID::COLOR) ? block->getTag<Col3u>(Block::TagID::COLOR).to<Color>() : WHITE);
    col.a = 255;

    tilemap->drawTilePro((uint16_t)block->getID() - 1, dest, col);
}

void RenderManager::renderEntity(std::string& textureKey, Entity* entity) {
    auto tm = TextureManager::get();
    auto tex = tm->getTexture(textureKey);

    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, entity->getHitbox().getRect().to<Rectangle>(), {0, 0}, 0.0f, WHITE);
}

void RenderManager::renderSimplePlayer(std::shared_ptr<SimplePlayer> player) {
    auto dbg = Debug::get();
    if(dbg->isVisible()) {
        for(auto& hitbox : m_world->getHitboxes(player->getHitbox())) {
            DrawRectangleLinesEx(hitbox.getRect().to<Rectangle>(), 0.05f, RED);
        }
    }
    
    auto tex = TextureManager::get()->getTexture("player.png");

    float frameWidth = tex.width / 17;
    auto hitbox = player->getHitbox();
    auto dir = (player->getDirection() == Entity::Direction::LEFT ? 1.0f : -1.0f);
    
    Rectangle src = {
        player->getAnimCurrentFrame() * frameWidth, 0, 
        frameWidth * dir, static_cast<float>(tex.height)
    };

    Rectangle dest = {
        hitbox.x + (hitbox.width / 2) - (hitbox.width * 1.45f) / 2, 
        hitbox.y, 
        hitbox.width * 1.45f, 
        hitbox.height
    };
    DrawTexturePro(tex, src, dest, {0, 0}, 0, WHITE);

    auto username = player->getUsername();
    if(!username.empty()) {
        auto size = MeasureTextEx(GetFontDefault(), username.c_str(), 0.5f, 0.05f);
        DrawTextEx(GetFontDefault(), username.c_str(), {hitbox.x + hitbox.width / 2.f - size.x / 2.f, hitbox.y - 1.f}, 0.5f, 0.05f, WHITE);
    }

    if(dbg->isVisible()) {
        DrawRectangleLinesEx(hitbox.getRect().to<Rectangle>(), 0.025f, GREEN);
    }
}