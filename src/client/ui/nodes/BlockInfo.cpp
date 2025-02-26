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

    if (block != nullptr) {
        Rectf blockDest = {10, (getSize().y - 16) / 2, 16, 16};

        std::string top = std::format("{} {} H: {}", Block::idToString(block->getID()), static_cast<int>(block->getID()), block->getDurability());
        std::string bot = std::format("X: {} Y: {}", target.x, target.y);

        RenderManager::renderBlock(blockDest, block);
        RenderManager::drawText("font", top, {35, 15});
        RenderManager::drawText("font", bot, {35, 1});
        setSize({static_cast<float>(RenderManager::getTextSize(top, "font").x + blockDest.x + blockDest.width + m_border * 7), 40});
    }
}