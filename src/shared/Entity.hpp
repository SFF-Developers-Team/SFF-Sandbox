#pragma once

#include "Box.hpp"
#include "Types.hpp"
#include "World.hpp"
#include <Vector2.hpp>

class Entity : public Box {
public:
    Entity(World& world);

    virtual void Update();

    RVector2& GetSpeed() noexcept { return m_speed; }
    bool ShouldRemove() const noexcept { return m_shouldRemove; }

    void MoveInWorld(float x, float y);

protected:
    RVector2 m_speed;
    bool m_shouldRemove;
    bool m_onGround;
    Direction m_direction;
    World& m_world;
};