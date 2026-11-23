#include <raylib-cpp.hpp>
#include <entity/LocalPlayer.hpp>
#include "world/Tile.hpp"
#include "world/World.hpp"
#include <memory>
#include <world/ClientWorld.hpp>
#include <managers/Debug.hpp>
#include <managers/RenderManager.hpp>
#include <entity/ClientPlayer.hpp>
#include <managers/SettingsManager.hpp>
#include <managers/ResourceManager.hpp>

ClientWorld::ClientWorld(std::filesystem::path const& saveDir) : World(saveDir) {}

void ClientWorld::draw(std::shared_ptr<LocalPlayer> player) {
    auto dbg = Debug::get();
    auto chunkCount = 0;
    auto tilesCount = 0;

    auto& camera = player->getCamera();
    raylib::Vector2 min = camera.GetScreenToWorld(raylib::Vector2::Zero());
    raylib::Vector2 max = camera.GetScreenToWorld(raylib::Window::GetSize());

    auto& rm = ResourceManager::Get();
    auto& gui = rm.GetTilemap("gui.png");

    for (int y = min.y; y < max.y; y++) {
        for (int x = min.x; x < max.x; x++) {
            TileID tile0 = getTile({x, y, 0});
            TileID tile1 = getTile({x, y, 1});

            if (tile1 != TileID::AIR) {
                drawTile(raylib::Rectangle(float(x), float(y), 1.f, 1.f), TilePosition{x, y, 1});
                continue;
            }

            if (tile0 != TileID::AIR) {
                drawTile(raylib::Rectangle(float(x), float(y), 1.f, 1.f), TilePosition{x, y, 0});
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
                    RenderManager::drawRectLines({h.x, h.y, h.width, h.height}, raylib::Color::Red(), 0.05f);
                }
            }
        }
    }

    for (auto& [playerId, info] : m_breakInfo) {
        raylib::Rectangle tileRect = {
            static_cast<float>(info.pos.x), static_cast<float>(info.pos.y), 
            1.f, 1.f
        };

        if (player->isRectInView(tileRect)) {
            auto tile = getTile(info.pos);
            gui.DrawTile(int(8 + (info.progress / Tile::getDurability(tile)) * 5), tileRect);   
        }
    }

    if (dbg->isVisible()) {
        dbg->setString(DebugID::RENDER_CHUNKS, "Chunks rendered: {}", chunkCount);
        dbg->setString(DebugID::RENDER_TILES, "Tiles rendered: {}", tilesCount);
    }
}

void ClientWorld::drawTile(raylib::Rectangle dest, TilePosition position) {
    TileID tile = getTile(position);

    if (tile != TileID::AIR) {
        auto sm = SettingsManager::get();
        
        raylib::Color col{255, 255, 255};

        if(position.layer < 1) {
            col = col.Brightness(-(1.f - sm->getValue<float>("video.layer0.brightness", 0.75f)));
        }

        ResourceManager::Get().GetTilemap("tiles.png").DrawTile((int)tile - 1, dest, col);
    }
}