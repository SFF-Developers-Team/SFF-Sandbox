#pragma once
#include <vector>
#include <Types.hpp>
#include <SerializedObject.hpp>

class World;

class Entity : public SerializedObject {
protected:
    Rectf m_hitbox = {0, 0, 16, 16};
    Vec2f m_speed = {0.0f, 0.0f};
    World* m_world;

    bool m_enabledPhysics = false;
    bool m_remove = false;
    float m_gravitation = 400.f;

public:
    Entity(World* world);
    Entity(World* world, Vec2f position, bool enablePhysics = false);

    virtual void update();
    void updatePhysics();

    virtual void processPhysics(bool hitWall, bool hitFloor, bool hitCeil) = 0;

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);

    inline void setPosition(Vec2f pos) {
        m_hitbox.x = pos.x;
        m_hitbox.y = pos.y;
    }

    inline Vec2f getPosition() {
        return Vec2f {m_hitbox.x, m_hitbox.y};
    }

    inline void remove() {
        m_remove = true;
    }

    bool willRemove() {
        return m_remove;
    }

    Rectf getHitbox() {
        return m_hitbox;
    }
};