#include <entity/OnlinePlayer.hpp>
#include <algorithm>
#include <raylib.h>

OnlinePlayer::OnlinePlayer(std::shared_ptr<World> world) : SimplePlayer(world), m_index(0) {}

void OnlinePlayer::update() {
    if (m_index > 0) {
        float time = GetTime() - 0.1f;
        float t = (time - m_positions[0].time) / (m_positions[1].time - m_positions[0].time);
        
        m_hitbox.x = std::lerp(m_positions[0].x, m_positions[1].x, t);
        m_hitbox.y = std::lerp(m_positions[0].y, m_positions[1].y, t);
    }
}

size_t OnlinePlayer::deserialize(ByteVector const& bytes) {
    SimplePlayer::deserialize(bytes);

    m_positions[m_index] = {m_hitbox.x, m_hitbox.y, static_cast<float>(GetTime())};
    m_index = (m_index + 1) % m_positions.size();

    return m_offset;
}