#include <entity/LocalPlayer.hpp>
#include "world/Tile.hpp"
#include "world/World.hpp"
#include <memory>
#include <world/ClientWorld.hpp>
#include <managers/Debug.hpp>
#include <managers/RenderManager.hpp>
#include <entity/ClientPlayer.hpp>
#include <raylib.h>
#include <raymath.h>

ClientWorld::ClientWorld(std::filesystem::path const& saveDir) : World(saveDir) {}

void ClientWorld::draw(std::shared_ptr<LocalPlayer> player) {
    auto dbg = Debug::get();
    auto chunkCount = 0;
    auto tilesCount = 0;

    Camera2D& camera = player->getCamera();
    Vector2 min = GetScreenToWorld2D(Vector2Zero(), camera);
    Vector2 max = GetScreenToWorld2D(Vector2 {float(GetScreenWidth()), float(GetScreenHeight())}, camera);

    for (int y = min.y; y < max.y; y++) {
        for (int x = min.x; x < max.x; x++) {
            TileID tile0 = getTile({x, y, 0});
            TileID tile1 = getTile({x, y, 1});

            if (tile1 != TileID::AIR) {
                RenderManager::drawWorldTile(Rectf {float(x), float(y), 1.f, 1.f}, {x, y, 1}, tile1);
                continue;
            }

            if (tile0 != TileID::AIR) {
                RenderManager::drawWorldTile(Rectf {float(x), float(y), 1.f, 1.f}, {x, y, 0}, tile0);
            }
        }
    }

    for (auto& [_, player] : getPlayers()) {
        auto clPlayer = std::dynamic_pointer_cast<ClientPlayer>(player);

        if (clPlayer != nullptr) {
            clPlayer->draw();

            if (dbg->isVisible()) {
                auto env = getHitboxes(player->getHitbox());

                for (auto& h : env) {
                    RenderManager::drawRectLines(h.getRect(), COL_RED, 0.05f);
                }
            }
        }
    }

    for (auto& [playerId, info] : m_breakInfo) {
        Rectf tileRect = {
            static_cast<float>(info.pos.x), static_cast<float>(info.pos.y), 
            1.f, 1.f
        };

        if (player->isRectInView(tileRect)) {
            auto tile = getTile(info.pos);
            RenderManager::drawTile("gui.png", 8 + (info.progress / Tile::getDurability(tile)) * 5.f, tileRect);   
        }
    }

    if (dbg->isVisible()) {
        dbg->setString(DebugID::RENDER_CHUNKS, "Chunks rendered: {}", chunkCount);
        dbg->setString(DebugID::RENDER_TILES, "Tiles rendered: {}", tilesCount);
    }
}