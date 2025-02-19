#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <entity/Player.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <TileMap.hpp>
#include <Types.hpp>
#include <Debug.hpp>
#include <Game.hpp>
#include <assert.h>
#include <Utils.hpp>
#include <string>

int blocksDrawn = 0;

void RenderManager::drawTexture(std::string const& key, Rectf dest, Col4u color, float rot, Vec2f origin) {
    auto texture = TextureManager::get()->getTexture(key);
    DrawTexturePro(texture, 
        {0.f, 0.f, static_cast<float>(texture.width), static_cast<float>(texture.height)}, 
        dest.to<Rectangle>(), 
        origin.to<Vector2>(), rot, 
        color.to<Color>()
    );
}

void RenderManager::drawTile(std::string const& mapKey, uint16_t index, Rectf dest, Col4u color, float rot, Vec2f origin) {
    auto tm = TextureManager::get();
    auto tilemap = tm->getTileMap(mapKey);

    DrawTexturePro(tilemap->getMap(), 
        tilemap->getRectForTile(index), 
        dest.to<Rectangle>(), 
        origin.to<Vector2>(), rot, 
        color.to<Color>()
    );
}

void RenderManager::drawText(std::string const& fontKey, std::string const& text, Vec2f pos, Col4u color, float fontSize, Vec2f origin, float spacing) {
    auto tm = TextureManager::get();
    auto font = tm->getFont(fontKey);

    if(fontSize == 0.f) {
        fontSize = font.baseSize;
    }

    if(origin.x != 0.f || origin.y != 0.f) { // Эта оптимизация направлена на пропуск подсчета размера текста, если точка опоры равна нулю
        auto size = getTextSize(text, fontKey, fontSize, spacing);
        pos.x -= size.x * origin.x;
        pos.y -= size.y * origin.y;
    }

    DrawTextEx(font, text.c_str(), {roundf(pos.x), roundf(pos.y)}, fontSize, spacing, color.to<Color>());
}

void RenderManager::drawRect(Rectf rect, Col4u col) {
    DrawRectangleRec(rect.to<Rectangle>(), col.to<Color>());
}

void RenderManager::drawRectLines(Rectf rect, Col4u col, float thick) {
    DrawRectangleLinesEx(rect.to<Rectangle>(), thick, col.to<Color>());
}

void RenderManager::drawFrame(Rectf rect, Col4u col, float borderSize) {
    drawRect(rect, col);
    
    if(borderSize > 0.f) {
        drawRectLines(rect, col - Col4u {0x7F, 0x7F, 0x7F, 0x7F}, borderSize);
    }
}

Vec2f RenderManager::getTextSize(std::string const& text, std::string const& fontKey, float fontSize, float spacing) {
    auto tm = TextureManager::get();
    auto font = tm->getFont(fontKey);
    auto size = MeasureTextEx(font, text.c_str(), fontSize, spacing);

    return Vec2f {size.x, size.y}; 
}

void RenderManager::renderWorld(std::shared_ptr<World> world, std::shared_ptr<Player> player) {
    auto dbg = Debug::get();
    auto chunksCount = 0;
    auto playersCount = 0;

    blocksDrawn = 0;

    for (auto& [pos, chunk] : world->getChunks()) {
        if (player->isChunkInView(chunk)) {
            renderChunk(chunk, player);
            chunksCount++;
        }

        auto layer = !IsKeyDown(KEY_LEFT_ALT);
        auto target = player->getTargetBlock();

        // Selected block
        if (player->canAccessBlock(target, layer)) {
            if(player->canPlaceBlock(target, layer)) {
                drawTile("gui.png", 1, BLOCK_RECT(target.x, target.y));
            }

            if(player->canDestroyBlock(target, layer)) {
                drawTile("gui.png", 2 + IsKeyDown(KEY_LEFT_CONTROL), BLOCK_RECT(target.x, target.y));
            }
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
    auto target = player->getTargetBlock();
    auto layer = IsKeyDown(KEY_LEFT_ALT);
    auto cpos = chunk->getPosition();

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < world->getHeight(); y++) {
            auto block0 = chunk->getBlock(x, y, 0);
            auto block1 = chunk->getBlock(x, y, 1);

            if (player->isBlockInView(block0) || player->isBlockInView(block1)) {
                auto blockX = ((cpos > 0 || cpos < 0) && x < CHUNK_WIDTH ? static_cast<float>(cpos) * CHUNK_WIDTH + x : x);

                auto watchAltBlock = 
                    target.x == blockX && 
                    target.y == y && 
                    IsKeyDown(KEY_LEFT_ALT) && 
                    player->canAccessBlock(target, layer);

                if (block0 != nullptr && (block1 == nullptr || watchAltBlock)) {
                    renderBlock(BLOCK_RECT(blockX, y), block0);
                    blocksDrawn++;
                }

                if (block1 != nullptr) {
                    renderBlock(BLOCK_RECT(blockX, y), block1, (watchAltBlock ? 128 : 255));
                    blocksDrawn++;
                }
            }
        }
    }

    if (dbg->isVisible()) {
        dbg->setString(RENDER_BLOCKS, "Blocks rendered: {}", blocksDrawn);
    }
}

void RenderManager::renderBlock(Rectf dest, std::shared_ptr<Block> block, uint8_t alpha) {
    assert(block != nullptr);

    auto index = static_cast<uint16_t>(block->getID() - 1);
    Col3u col = {255, 255, 255};

    if (block->hasTag(Block::TagID::COLOR)) { 
        col = block->getTag<Col3u>(Block::TagID::COLOR);
    }

    if(!block->getLayer()) {
        col.brightness(-0.25f);
    }

    drawTile("blocks.png", index, dest, {col.r, col.g, col.b, alpha});
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