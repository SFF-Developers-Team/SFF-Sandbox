#include <ui/nodes/HeartsIndicator.hpp>
#include <RenderManager.hpp>
#include <entity/Player.hpp>
#include <raylib.h>

HeartsIndicator::HeartsIndicator(std::shared_ptr<Player> player) : m_player(player) {
    setSize({m_player->getMaxHealth() * 5.f, 7});
}

void HeartsIndicator::draw() {
    auto add = (GetTime() < m_player->getLastHeartTime() + 0.25f) * 3;

    for(float i = 0; i <= m_player->getMaxHealth() / 2.f; i++) {
        if(m_player->getHealth() / 2.f > i + 1.f) {
            RenderManager::drawTile("indicators.png", add, {i * 8.f + i, 0.f, 8.f, 7.f});
            continue;
        }

        if(m_player->getHealth() / 2.f > i + 0.5f) {
            RenderManager::drawTile("indicators.png", 1 + add, {i * 8.f + i, 0.f, 8.f, 7.f});
            continue;
        }

        RenderManager::drawTile("indicators.png", 2 + add, {i * 8.f + i, 0.f, 8.f, 7.f});
    }
}