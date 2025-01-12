#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <entity/Player.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <TileMap.hpp>
#include <Types.hpp>
#include <Debug.hpp>
#include <Game.hpp>

#include <string>

RenderManager::RenderManager() {}

void RenderManager::drawTexture(std::string const& key, Rectf dest, Col4u color, float rot, Vec2f origin) {
    auto texture = TextureManager::get()->getTexture(key);
    DrawTexturePro(texture, 
        {0.f, 0.f, static_cast<float>(texture.width), static_cast<float>(texture.height)}, 
        dest.to<Rectangle>(), 
        origin.to<Vector2>(), rot, 
        color.to<Color>()
    );
}

void RenderManager::renderWorld(std::shared_ptr<World> world, std::shared_ptr<Player> player) {
    auto dbg = Debug::get();
    auto chunksCount = 0;
    auto playersCount = 0;

    for (auto& [pos, chunk] : world->getChunks()) {
        if (player->isChunkInView(chunk)) {
            renderChunk(chunk, player);
            chunksCount++;
        }

        auto layer = IsKeyDown(KEY_LEFT_ALT);
        auto target = player->getTargetBlock();

        // Selected block
        if (player->canAccessBlock(target, layer)) {
            DrawRectangleLinesEx({(float)target.x, (float)target.y, 1.0f, 1.0f}, 1.0f / 16.f, WHITE);
        }

        if (dbg->isVisible()) {
            DrawLineV({(float)pos * CHUNK_WIDTH, 0.f}, {(float)pos * CHUNK_WIDTH, 256.f}, YELLOW);

            DrawLineV({(float)pos * CHUNK_WIDTH + CHUNK_WIDTH, 0.f}, {(float)pos * CHUNK_WIDTH + CHUNK_WIDTH, 256.f}, YELLOW);

            dbg->setString(RENDER_CHUNKS, "Chunks rendered: {}", chunksCount);
            
        }
    }

    for (auto& [_, player] : world->getPlayers()) {
        renderSimplePlayer(player);
        dbg->setString(RENDER_PLAYERS, "Players rendered: {}", playersCount++);
    }
}

void RenderManager::renderChunk(std::shared_ptr<Chunk> chunk, std::shared_ptr<Player> player) {
    auto game = Game::get();
    auto dbg = Debug::get();
    auto world = chunk->getWorld();
    auto tilemap = game->getBlocksTileMap();
    auto blocksDrawn = 0;
    auto target = player->getTargetBlock();
    auto layer = IsKeyDown(KEY_LEFT_ALT);
    auto cpos = chunk->getPosition();

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < world->getHeight(); y++) {
            auto block0 = chunk->getBlock(x, y, 0);
            auto block1 = chunk->getBlock(x, y, 1);

            // Пока что все блоки у нас одинакового размера
            if (player->isBlockInView(block0)) {
                auto blockX = ((cpos > 0 || cpos < 0) && x < CHUNK_WIDTH ? static_cast<float>(cpos) * CHUNK_WIDTH + x : x);

                auto watchAltBlock = 
                    target.x == blockX && 
                    target.y == y && 
                    IsKeyDown(KEY_LEFT_ALT) && 
                    player->canAccessBlock(target, layer);

                if (block0 && ((block1 && block1->getID() == Block::ID::AIR) || watchAltBlock)) {
                    renderBlock(blockX, y, block0);
                    blocksDrawn++;
                }

                if (block1) {
                    renderBlock(blockX, y, block1, (watchAltBlock ? 128 : 255));
                    blocksDrawn++;
                }
            }
        }
    }

    if (dbg->isVisible()) {
        dbg->setString(RENDER_BLOCKS, "Blocks rendered: {}", blocksDrawn);
    }
}

void RenderManager::renderBlock(float x, float y, std::shared_ptr<Block> block, uint8_t alpha) {
    if (block->getID() == Block::ID::AIR) {
        return;
    }

    auto tilemap = Game::get()->getBlocksTileMap();
    auto dest = Rectangle {x, y, 1.0f, 1.0f};
    Color col = (block->hasTag(Block::TagID::COLOR) ? block->getTag<Col3u>(Block::TagID::COLOR).to<Color>() : WHITE);
    col.a = alpha;

    tilemap->drawTilePro((uint16_t)block->getID() - 1, dest, ColorBrightness(col, (block->getLayer() == 0 ? -0.25f : 0.0f)));
}

void RenderManager::renderUIBlock(float x, float y, float width, float height, std::shared_ptr<Block> block) {
    if (!block || block->getID() == Block::ID::AIR) {
        return;
    }

    auto tilemap = Game::get()->getBlocksTileMap();
    auto dest = Rectangle {x, y, width, height};
    Color col = (block->hasTag(Block::TagID::COLOR) ? block->getTag<Col3u>(Block::TagID::COLOR).to<Color>() : WHITE);
    col.a = 255;

    tilemap->drawTilePro((uint16_t)block->getID() - 1, dest, col);
}

void RenderManager::renderEntity(std::string& textureKey, std::shared_ptr<Entity> entity) {
    auto tm = TextureManager::get();
    auto tex = tm->getTexture(textureKey);

    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, entity->getHitbox().getRect().to<Rectangle>(), {0, 0}, 0.0f, WHITE);
}

void RenderManager::renderSimplePlayer(std::shared_ptr<SimplePlayer> player) {
    auto dbg = Debug::get();
    if (dbg->isVisible()) {
        for (auto& hitbox : player->getWorld()->getHitboxes(player->getHitbox())) {
            DrawRectangleLinesEx(hitbox.getRect().to<Rectangle>(), 0.05f, RED);
        }
    }

    auto tex = TextureManager::get()->getTexture("player.png");

    float frameWidth = tex.width / 17;
    auto hitbox = player->getHitbox();
    auto dir = (player->getDirection() == Direction::LEFT ? 1.0f : -1.0f);

    Rectangle src = {player->getAnimCurrentFrame() * frameWidth, 0, frameWidth * dir, static_cast<float>(tex.height)};

    Rectangle dest = {hitbox.x + (hitbox.width / 2) - (hitbox.width * 1.45f) / 2, hitbox.y, hitbox.width * 1.45f, hitbox.height};
    DrawTexturePro(tex, src, dest, {0, 0}, 0, WHITE);

    auto username = player->getUsername();
    if (!username.empty()) {
        auto size = MeasureTextEx(GetFontDefault(), username.c_str(), 0.5f, 0.05f);
        DrawTextEx(GetFontDefault(), username.c_str(), {hitbox.x + hitbox.width / 2.f - size.x / 2.f, hitbox.y - 1.f}, 0.5f, 0.05f, WHITE);
    }

    if (dbg->isVisible()) {
        DrawRectangleLinesEx(hitbox.getRect().to<Rectangle>(), 0.025f, GREEN);
    }
}

 void RenderManager::renderPlayerTexture(Vec2f pos, std::string const& key, Vec2f size, int animFrame, Direction direction, std::string const& username, float nameHeight, float nameSpacing) {
    auto tex = TextureManager::get()->getTexture(key);

    float frameWidth = tex.width / 17;
    auto dir = (direction == Direction::LEFT ? 1.0f : -1.0f);
    Rectangle src = {animFrame * frameWidth, 0, frameWidth * dir, static_cast<float>(tex.height)};
    Rectangle dest = {pos.x, pos.y, size.x, size.y};

    DrawTexturePro(tex, src, dest, {0, 0}, 0, WHITE);

    if (!username.empty()) {
        auto size = MeasureTextEx(GetFontDefault(), username.c_str(), nameHeight, nameSpacing);
        DrawTextEx(GetFontDefault(), username.c_str(), {pos.x + size.x / 2.f - size.x / 2.f, pos.y - 1.f}, nameHeight, nameSpacing, WHITE);
    }
}