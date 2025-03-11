#pragma once
#include <Serializable.hpp>
#include <entity/Hitbox.hpp>
#include <Types.hpp>
#include <vector>
#include <memory>

class World;

class Entity : public Serializable {
protected:
    Hitbox m_hitbox;
    Vec2f m_speed;

    bool m_remove = false;
    bool m_onGround = false;

    Direction m_direction;

public:
    Entity();

    virtual void onTick() {}
    void move(std::vector<Hitbox>& env, float xa, float ya);

    DataStream serialize() override;
    bool deserialize(DataStream& stream) override;

    void setPosition(Vec2f pos);
    void setSize(Vec2f size);
    void setDirection(Direction dir) { m_direction = dir; }

    void remove() { m_remove = true; }

    auto getHitbox() { return m_hitbox; }
    auto getPosition() { return Vec2f {m_hitbox.x, m_hitbox.y}; }
    auto getDirection() { return m_direction; }
    auto getSize() { return Vec2f {m_hitbox.width, m_hitbox.height}; }
    auto willRemove() { return m_remove; }
    auto getSpeed() { return m_speed; }
};