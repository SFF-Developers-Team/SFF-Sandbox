#include <entity/ClientPlayer.hpp>
#include <managers/TextureManager.hpp>
#include <managers/RenderManager.hpp>
#include <managers/Debug.hpp>

ClientPlayer::ClientPlayer(std::shared_ptr<World> world) : Player(world) {}

void ClientPlayer::draw() {
    auto tex = TextureManager::get()->getTexture("player.png");
    int frameWidth = tex.width / 17;
    float scaleX = 1.45f;

    Rectangle src = {
        static_cast<float>(m_animFrame * frameWidth), 0.f, 
        static_cast<float>(frameWidth * m_direction), static_cast<float>(tex.height)
    };

    Rectangle dest = {
        m_hitbox.x, m_hitbox.y, 
        m_hitbox.width * scaleX, m_hitbox.height
    };

    DrawTexturePro(tex, src, dest, {(dest.width - m_hitbox.width) / 2.f, 0.f}, 0.f, WHITE);

    if (!m_username.empty()) {
        RenderManager::drawText("font", m_username, {m_hitbox.x + m_hitbox.width / 2.f, m_hitbox.y - 1.f}, COL_WHITE, 0.5f, {0.5f, 1.f}, 0.05f);
    }

    auto dbg = Debug::get();
    
    if (dbg->isVisible()) {
        RenderManager::drawRectLines(m_hitbox.getRect(), COL_GREEN);
    }
}