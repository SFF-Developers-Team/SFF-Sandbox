#pragma once

#include "Box.hpp"
#include "Types.hpp"
#include "World.hpp"
#include <Vector2.hpp>

class Entity : public Box {
public:
    Entity(World& world, RVector2 position);

    virtual void OnTick();

    RVector2& GetSpeed() noexcept { return m_speed; }
    float GetHealth() noexcept { return m_hp; }
    float GetLastTimeHurt() noexcept { return m_lastTimeHurt; }
    World&  GetWorld() { return m_world; }

    void SetHealth(int hp) { m_hp = hp; }
    void AddHealth(int hp) { if (hp < 20) { m_hp += hp; } else { m_hp = 20; } }

    bool ShouldRemove() const noexcept { return m_shouldRemove; }

    void MoveInWorld(float x, float y);
protected:
    RVector2 m_speed;
    bool m_shouldRemove;
    bool m_onGround;
    Direction m_direction;
    World& m_world;
    int m_groundY = 0;
    int m_lastTimeHurt = 0;
    int m_hp = 20;
};