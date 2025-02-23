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
    auto block = Game::get()->getWorld()->getBlock(target.x, target.y, target.layer);

    RenderManager::renderBlock({10, (getSize().y - 16) / 2, 16, 16}, block);
    RenderManager::drawText("font", std::format("{} {}", Block::idToString(block->getID()), static_cast<int>(block->getID())), {35, 15});
    RenderManager::drawText("font", std::format("X: {} Y: {}", target.x, target.y), {35, 1});
}