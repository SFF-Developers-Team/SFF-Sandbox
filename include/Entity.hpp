#pragma once
#include <raylib.h>
#include <vector>
#include <SerializedObject.hpp>

class World;

class Entity : public SerializedObject {
protected:
    Rectangle m_hitbox = {0, 0, 16, 16};
    Vector2 m_speed = {0.0f, 0.0f};
    Texture2D m_texture;
    World* m_world;
    std::vector<Rectangle> m_hitboxes;

    bool m_enabledPhysics = false;
    float m_gravitation = 400;

public:
    Entity(World* world);
    Entity(World* world, Vector2 position, bool enablePhysics = false);

    void update();
    void updatePhysics();
    void updateHitboxes();

    virtual void processPhysics(bool hitWall, bool hitFloor, bool hitCeil) = 0;

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);

    inline void setPosition(Vector2 pos) {
        m_hitbox.x = pos.x;
        m_hitbox.y = pos.y;
    }

    inline Vector2 getPosition() {
        return Vector2 {m_hitbox.x, m_hitbox.y};
    }
};