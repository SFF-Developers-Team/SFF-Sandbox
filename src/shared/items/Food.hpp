#pragma once

#include "UsableItem.hpp"
#include "Player.hpp"
template <int hp>

class Food : public UsableItem {
public:
    Food(int id, uint8_t spriteIndex) : UsableItem(id, spriteIndex) {}
    Food(Food const& food) : UsableItem(food.m_id, food.m_spriteIndex) {}

    bool OnUse(Player* player) override {
        auto& world = player->GetWorld();

        if (world.GetTicks() >= m_lastUse + 70) {
            player->AddHealth(hp);
            m_lastUse = world.GetTicks();

            return true;
        }
        return false;
    };

    Food& operator=(Food const& food) {
        m_id = food.m_id;
        m_spriteIndex = food.m_spriteIndex;

        return *this;
    }

private:
    int m_lastUse = 0;
};