#pragma once
#include <vector>

class World;
class Particle;

class ParticleManager {
private:
    std::vector<Particle*> m_particles;
    World* m_world;

public:
    ParticleManager(World* world);
    void add(Particle* particle);

    void update();
    void render();
};