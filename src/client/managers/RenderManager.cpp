#include <managers/TextureManager.hpp>
#include <managers/RenderManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/Chunk.hpp>
#include <world/ClientWorld.hpp>
#include <inventory/InventoryItem.hpp>
#include <TileMap.hpp>
#include <Types.hpp>
#include <managers/Debug.hpp>
#include <Game.hpp>
#include <assert.h>
#include <Utils.hpp>
#include <string>
#include <unordered_map>
#include <managers/SettingsManager.hpp>
#include <world/ClientChunk.hpp>
#include <rlgl.h>

std::unordered_map<char, Color> colorMap = {
    {'r', RED},
    {'g', GREEN},
    {'b', BLUE},
    {'0', BLACK},
    {'w', WHITE},
    {'y', YELLOW},
    {'o', ORANGE},
    {'p', PURPLE} 
};

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
    Vector2 anchor = {dest.width * origin.x, dest.height * origin.y};

    DrawTexturePro(tilemap->getMap(), 
        tilemap->getRectForTile(index), 
        dest.to<Rectangle>(), 
        anchor, rot, 
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

void RenderManager::drawColoredText(std::string const& fontKey, std::string const& text, Vec2f pos, Col4u color, float fontSize, Vec2f origin, float spacing) {
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

    const char* start = text.c_str();
    Color currentColor = WHITE;
    Vec2f curPos = pos;

    while (*start) {
        if (*start == '\\' && *(start + 1) == 'c' && *(start + 2)) {
            char colorCode = *(start + 2);

            if (colorMap.find(colorCode) != colorMap.end()) {
                currentColor = colorMap[colorCode];
            }

            start += 3;
        } else {
            const char* end = strstr(start, "\\c");
            if (!end) end = start + strlen(start);

            std::string part(start, end);
            DrawTextEx(font, part.c_str(), {roundf(curPos.x), roundf(curPos.y)}, fontSize, spacing, currentColor);

            auto size = getTextSize(part, fontKey, fontSize, spacing);
            
            if (size.y > fontSize) {
                curPos.x = pos.x;
                curPos.y += size.y - fontSize;
            } else {
                curPos.x += size.x;
            }

            start = end;
        }
    }
}

void RenderManager::drawRect(Rectf rect, Col4u col) {
    DrawRectangleRec(rect.to<Rectangle>(), col.to<Color>());
}

void RenderManager::drawRectLines(Rectf rect, Col4u col, float thick) {
    if (thick == 0.f) {
        auto mat = rlGetMatrixModelview();
        thick = 1.f / mat.m0;
    }

    DrawRectangleLinesEx(rect.to<Rectangle>(), thick, col.to<Color>());
}

void RenderManager::drawFrame(Rectf rect, Col4u col, float borderSize) {
    drawRect(rect, col);

    TO_BORDER_COLOR(col);
    
    if(borderSize > 0.f) {
        drawRectLines(rect, col, borderSize);
    }
}

Vec2f RenderManager::getTextSize(std::string const& text, std::string const& fontKey, float fontSize, float spacing) {
    auto tm = TextureManager::get();
    auto font = tm->getFont(fontKey);

    if(fontSize == 0.f) {
        fontSize = font.baseSize;
    }
    
    auto size = MeasureTextEx(font, text.c_str(), fontSize, spacing);

    return Vec2f {size.x, size.y}; 
}

void RenderManager::renderBlock(Rectf dest, BlockPosition position, std::shared_ptr<Block> block, uint8_t alpha) {
    if (block != nullptr) {
        auto sm = SettingsManager::get();
        
        Col3u col = {255, 255, 255};

        if (block->hasTag(TagID::TAG_COLOR)) { 
            col = block->getTag<Col3u>(TagID::TAG_COLOR);
        }

        if(position.layer < 1) {
            col.brightness(-(1.f - sm->getValue<float>("video.layer0.brightness", 0.75f)));
        }

        drawTile("blocks.png", block->getSpriteIndex(), dest, {col.r, col.g, col.b, alpha});
    }
}

void RenderManager::renderInventoryItem(Rectf dest, std::shared_ptr<InventoryItem> item) {
    Col3u col = {255, 255, 255};

    if (item->hasTag(TagID::TAG_COLOR)) { 
        col = item->getTag<Col3u>(TagID::TAG_COLOR);
    }

    drawTile((item->getType() == TYPE_BLOCK) ? "blocks.png" : "items.png", item->getSpriteIndex(), dest, {col.r, col.g, col.b, 255});

    if(item->getCount() > 1) {
        RenderManager::drawText("font", std::to_string(item->getCount()), {dest.x + dest.width, dest.y + dest.height}, COL_WHITE, 0.f, {1.f, 1.f});
    }
}

void RenderManager::renderEntity(std::string& textureKey, std::shared_ptr<Entity> entity) {
    auto tm = TextureManager::get();
    auto tex = tm->getTexture(textureKey);

    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, entity->getHitbox().getRect().to<Rectangle>(), {0, 0}, 0.0f, WHITE);
}

void RenderManager::renderPlayerTexture(Vec2f pos, std::string const& key, Vec2f size, int animFrame, Direction direction, std::string const& username, float nameHeight, float nameSpacing) {
    auto tex = TextureManager::get()->getTexture(key);

    int frameWidth = tex.width / 17;

    Rectangle src = {
        static_cast<float>(animFrame * frameWidth), 0.f, 
        static_cast<float>(frameWidth * direction), static_cast<float>(tex.height)
    };

    Rectangle dest = {pos.x, pos.y, size.x, size.y};

    DrawTexturePro(tex, src, dest, {0, 0}, 0, WHITE);

    if (!username.empty()) {
        RenderManager::drawText("font", username, {pos.x + size.x / 2.f, pos.y - 1.f}, COL_WHITE, 0.5f, {0.5f, 1.f}, 0.05f);
    }
}