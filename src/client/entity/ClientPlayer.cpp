#include <entity/ClientPlayer.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/RenderManager.hpp>
#include <managers/Debug.hpp>

ClientPlayer::ClientPlayer(World& world) : Player(world) {}

void ClientPlayer::draw() {
    auto& rm = ResourceManager::Get();
    auto& tex = rm.GetTexture("player.png");
    int frameWidth = tex.width / 17;
    float scaleX = 1.45f;

    raylib::Rectangle src = {
        static_cast<float>(m_animFrame * frameWidth), 0.f, 
        static_cast<float>(frameWidth * m_direction), static_cast<float>(tex.height)
    };

    raylib::Rectangle dest = {
        m_hitbox.x, m_hitbox.y, 
        m_hitbox.width * scaleX, m_hitbox.height
    };

    tex.Draw(src, dest, Vector2 {(dest.width - m_hitbox.width) / 2.f, 0.f}, 0.f, raylib::Color::White());

    if (!m_username.empty()) {
        auto& font = rm.GetFont("font");
        float fontSize = 0.5f;
        auto width = font.MeasureText(m_username, fontSize, 0.05f).x;
        font.DrawText(m_username, {m_hitbox.x + m_hitbox.width / 2.f - width / 2.f, m_hitbox.y - fontSize - 1.f},
                      fontSize, 0.05f, raylib::Color::White());
    }

    auto dbg = Debug::get();
    
    if (dbg->isVisible()) {
        RenderManager::drawRectLines({m_hitbox.x, m_hitbox.y, m_hitbox.width, m_hitbox.height}, raylib::Color::Green());
    }
}