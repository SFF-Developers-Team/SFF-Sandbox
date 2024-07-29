#include <ParticleManager.hpp>
#include <Particle.hpp>

ParticleManager::ParticleManager(World* world) : m_world(world) {}

void ParticleManager::add(Particle* particle) {
    m_particles.push_back(particle);
}

void ParticleManager::update() {
    for(auto i = 0; i < m_particles.size(); i++) {
        m_particles[i]->update();
        
        if(m_particles[i]->willRemove()) {
            delete m_particles[i];
            m_particles.erase(m_particles.begin() + i);
        }
    }
}

void ParticleManager::render() {
    for(auto& particle : m_particles) {
        particle->draw();
    }
}