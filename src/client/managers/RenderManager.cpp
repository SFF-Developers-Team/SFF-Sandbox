#include <managers/ResourceManager.hpp>
#include <managers/RenderManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/ClientWorld.hpp>
#include <Tilemap.hpp>
#include <Types.hpp>
#include <managers/Debug.hpp>
#include <Game.hpp>
#include <string>
#include <managers/SettingsManager.hpp>
#include <rlgl.h>

void RenderManager::drawRectLines(raylib::Rectangle rect, raylib::Color col, float thick) {
    if (thick == 0.f) {
        auto mat = rlGetMatrixModelview();
        thick = 1.f / mat.m0;
    }

    rect.DrawLines(col, thick);
}

void RenderManager::drawFrame(raylib::Rectangle rect, raylib::Color col, float borderSize) {
    rect.Draw(col);
    
    if(borderSize > 0.f) {
        drawRectLines(rect, col.Brightness(-0.5f), borderSize);
    }
}


void RenderManager::renderInventoryItem(raylib::Rectangle dest, Item const& item, int count) {
    auto id = item.isTileItem() ? item.getID() : item.getID() - ITEM_ID_BEGIN;
    auto& rm = ResourceManager::Get();
    auto& tilemap = rm.GetTilemap(item.isTileItem() ? "tiles.png" : "items.png");
    auto& font = rm.GetFont("font");

    tilemap.DrawTile(id - 1, dest);

    if(count > 1) {
        auto text = std::to_string(count);
        auto fontSize = 10.f;
        auto bounds = font.MeasureText(text, fontSize, 1.f);
        font.DrawText(text, {dest.x + dest.width - bounds.x, dest.y + dest.height - bounds.y},
                      fontSize, 1.f, raylib::Color::White());
    }
}

void RenderManager::renderEntity(std::string& textureKey, std::shared_ptr<Entity> entity) {
    auto& tm = ResourceManager::Get();
    auto& tex = tm.GetTexture(textureKey);

    tex.Draw(raylib::Rectangle {0.f, 0.f, float(tex.width), float(tex.height)}, raylib::Rectangle{entity->getHitbox().x, entity->getHitbox().y, entity->getHitbox().width, entity->getHitbox().height});
}

void RenderManager::renderPlayerTexture(raylib::Vector2 pos, std::string const& key, raylib::Vector2 size, int animFrame, Direction direction, std::string const& username, float nameHeight, float nameSpacing) {
    auto& rm = ResourceManager::Get();
    auto& tex = rm.GetTexture(key);
    auto& font = rm.GetFont("font");

    int frameWidth = tex.width / 17;

    raylib::Rectangle src = {
        float(animFrame * frameWidth), 0.f, 
        float(frameWidth * direction), float(tex.height)
    };

    raylib::Rectangle dest = {pos.x, pos.y, size.x, size.y};

    tex.Draw(src, dest, {0, 0}, 0, raylib::Color::White());

    if (!username.empty()) {
        const float fontSize = 0.5f;
        const float spacing = 0.05f;
        auto width = font.MeasureText(username, fontSize, spacing).x;
        font.DrawText(username, {pos.x + size.x * 0.5f - width * 0.5f, pos.y - fontSize},
                      fontSize, spacing, raylib::Color::White());
    }
}