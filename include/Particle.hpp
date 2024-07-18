#pragma once
#include <raylib.h>
#include <Entity.hpp>
#include <World.hpp>

class Particle : public Entity {
private:
    int m_lifeTime;
    int m_age = 0;

public:
    Particle(World* world, Vector2 pos, Vector2 speed, Texture2D texture);

    void processPhysics(bool, bool, bool) override;

    void update();
    void draw();
};