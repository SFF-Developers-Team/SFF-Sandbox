// #include <Particle.hpp>

// Particle::Particle(World* world, Vec2f pos, Vec2f speed, Texture2D texture) : Entity::Entity(world) {
//     m_texture = texture;
//     m_speed = speed;

//     m_hitbox.width = 2.f;
//     m_hitbox.height = 2.f;

//     m_lifeTime = (int)(4.0f / ((rand() / RAND_MAX) * 0.9f + 0.1f));
// }

// void Particle::update() {
//     Entity::update();
// }

// void Particle::processPhysics(bool, bool, bool) {
//     m_speed.x *= 0.98f;

//     if(m_speed.y == 0.0f) {
//         m_speed.x *= 0.7f;
//     }

//     if(m_age++ >= m_lifeTime) {
//         remove();
//     }
// }

// void Particle::draw() {
//     DrawTextureRec(m_texture, {(rand() / RAND_MAX) * (float)m_texture.width, (rand() / RAND_MAX) * (float)m_texture.height, 2, 2}, {m_hitbox.x, m_hitbox.y}, WHITE);
// }