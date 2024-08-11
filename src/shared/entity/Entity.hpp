#pragma once
#include <vector>
#include <Types.hpp>
#include <Hitbox.hpp>
#include <SerializedObject.hpp>

class World;

class Entity : public SerializedObject {
public:
    enum Direction : uint8_t {
        LEFT = 0,
        RIGHT = 1
    };

protected:
    Hitbox m_hitbox;
    World* m_world;

    float m_prevX;
    float m_prevY;
    float m_speedX = 0.0f;
    float m_speedY = 0.0f;

    bool m_remove = false;
    bool m_onGround = false;

    Direction m_direction;

    void resetPosition();
    void moveRelative(float x, float speed);

public:
    Entity(World* world);

    virtual void onTick();
    void move(float x, float y);

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);

    void setPosition(Vec2f pos);
    void setSize(Vec2f size);
    void turn(Direction dir);

    void remove() { m_remove = true; }

    auto getHitbox() { return m_hitbox; }
    auto getPosition() { return Vec2f {m_hitbox.x, m_hitbox.y}; }
    auto getDirection() { return m_direction; }
    auto getSize() { return Vec2f {m_hitbox.width, m_hitbox.height}; }
    auto willRemove() { return m_remove; }
    auto getSpeed() { return Vec2f {m_speedX, m_speedY}; }
};