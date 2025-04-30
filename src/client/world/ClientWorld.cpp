#include "world/Block.hpp"
#include "world/World.hpp"
#include <memory>
#include <world/ClientWorld.hpp>
#include <world/ClientChunk.hpp>
#include <entity/LocalPlayer.hpp>
#include <cassert>
#include <managers/Debug.hpp>
#include <managers/RenderManager.hpp>
#include <entity/ClientPlayer.hpp>

ClientWorld::ClientWorld(std::filesystem::path const& saveDir) : World(saveDir) {}

void ClientWorld::addChunk(Vec2i pos, std::shared_ptr<Chunk> chunk) {
    auto clChunk = std::make_shared<ClientChunk>(std::move(*chunk));
    clChunk->rebuild();
    World::addChunk(pos, clChunk);
}

void ClientWorld::draw(std::shared_ptr<LocalPlayer> player) {
    auto dbg = Debug::get();
    auto chunkCount = 0;
    auto blockCount = 0;
    auto playersCount = 0;

    for (auto& [pos, chunk] : getChunks()) {
        if (player->isChunkInView(pos)) {
            auto clChunk = std::dynamic_pointer_cast<ClientChunk>(chunk);
            clChunk->draw(pos);

            chunkCount++;
            blockCount += clChunk->getBlockCount();
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
            RenderManager::drawTile("gui.png", 8 + (info.progress / block->getDurability()) * 5.f, blockRect);   
        }
    }

    if (dbg->isVisible()) {
        dbg->setString(DebugID::RENDER_CHUNKS, "Chunks rendered: {}", chunkCount);
        dbg->setString(DebugID::RENDER_BLOCKS, "Blocks rendered: {}", blockCount);
    }
}