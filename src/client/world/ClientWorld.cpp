#include <entity/LocalPlayer.hpp>
#include "world/Block.hpp"
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
    auto blockCount = 0;
    auto playersCount = 0;

    Camera2D& camera = player->getCamera();
    Vector2 min = GetScreenToWorld2D(Vector2Zero(), camera);
    Vector2 max = GetScreenToWorld2D(Vector2 {float(GetScreenWidth()), float(GetScreenHeight())}, camera);

    for (int y = min.y; y < max.y; y++) {
        for (int x = min.x; x < max.x; x++) {
            BlockID block0 = getBlock({x, y, 0});
            BlockID block1 = getBlock({x, y, 1});

            if (block1 != BlockID::AIR) {
                RenderManager::renderBlock(Rectf {float(x), float(y), 1.f, 1.f}, {x, y, 1}, block1);
                continue;
            }

            if (block0 != BlockID::AIR) {
                RenderManager::renderBlock(Rectf {float(x), float(y), 1.f, 1.f}, {x, y, 0}, block0);
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
        Rectf blockRect = {
            static_cast<float>(info.pos.x), static_cast<float>(info.pos.y), 
            1.f, 1.f
        };

        if (player->isRectInView(blockRect)) {
            auto block = getBlock(info.pos);
            RenderManager::drawTile("gui.png", 8 + (info.progress / Block::getDurability(block)) * 5.f, blockRect);   
        }
    }

    if (dbg->isVisible()) {
        dbg->setString(DebugID::RENDER_CHUNKS, "Chunks rendered: {}", chunkCount);
        dbg->setString(DebugID::RENDER_BLOCKS, "Blocks rendered: {}", blockCount);
    }
}