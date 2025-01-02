#include <OnlinePlayer.hpp>

OnlinePlayer::OnlinePlayer(std::shared_ptr<World> world) : SimplePlayer(world) {};

void OnlinePlayer::onTick() {
    // if(m_prevX == m_hitbox.x && m_prevY == m_hitbox.y) {
    //     setAnimation(AnimationType::PLAYER_IDLE);
    //     m_animFrame = 0;
    // }

    Entity::onTick();
}