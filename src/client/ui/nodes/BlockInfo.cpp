#include <ui/nodes/BlockInfo.hpp>
#include <RenderManager.hpp>
#include <Game.hpp>
#include <entity/Player.hpp>
#include <world/World.hpp>
#include <world/Block.hpp>
#include <format>
void BlockInfo::draw() {
    Frame::draw();
    auto target = Game::get()->getPlayer()->getTargetBlock();
    auto layer = !IsKeyDown(KEY_LEFT_ALT);
    auto block = Game::get()->getWorld()->getBlock(target.x, target.y, layer);

    RenderManager::renderBlock({10, (getSize().y - 16) / 2, 16, 16}, block);
    RenderManager::drawText("font", std::format("{}", Block::idToString(block->getID())), {35, 15});
    RenderManager::drawText("font", std::format("{}", static_cast<int>(block->getID())), {static_cast<float>(50 + Block::idToString(block->getID()).size() * 6), 15});

    RenderManager::drawText("font", std::format("X: {}", target.x), {35, 1});
    RenderManager::drawText("font", std::format("Y: {}", target.y), {80, 1});
}

void BlockInfo::update() {
}
 