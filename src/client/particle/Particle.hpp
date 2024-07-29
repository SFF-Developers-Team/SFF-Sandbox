#pragma once
#include <raylib.h>
#include <Entity.hpp>
#include <World.hpp>

class Particle : public Entity {
private:
    int m_lifeTime;
    int m_age = 0;
    Texture2D m_texture;

public:
    Particle(World* world, Vec2f pos, Vec2f speed, Texture2D texture);

    void processPhysics(bool, bool, bool) override;

    void update() override;
    void draw();
};