#include <ui/gameplay/HeartsIndicator.hpp>
#include <managers/ResourceManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/World.hpp>

HeartsIndicator::HeartsIndicator(std::shared_ptr<LocalPlayer> player) : m_player(player) {
    // maxHealth = 20; spriteSize = 8;
    // maxHealth / 2 * spriteSize  
    setSize({m_player->getMaxHealth() * 4.5f, 7});
}

void HeartsIndicator::draw() {
    // auto add = (m_player->getWorld()->getTime() < m_player->getLastHurtTime() + 0.5f) * 3;
    auto add = 0;

    auto& rm = ResourceManager::Get();

    for(auto i = 0; i < m_player->getMaxHealth() / 2.f; i++) {
        if(m_player->getHealth() / 2.f >= i + 1.f) {
            rm.GetTilemap("indicators.png").DrawTile(add, raylib::Rectangle{i * 8.f + i, 0.f, 8.f, 7.f});
            continue;
        }

        if(m_player->getHealth() / 2.f >= i + 0.5f) {
            rm.GetTilemap("indicators.png").DrawTile(1 + add, raylib::Rectangle{i * 8.f + i, 0.f, 8.f, 7.f});
            continue;
        }

        rm.GetTilemap("indicators.png").DrawTile(2 + add, raylib::Rectangle{i * 8.f + i, 0.f, 8.f, 7.f});
    }
}