#include "ResourceManager.hpp"
#include <Player.hpp>
#include <Rectangle.hpp>

void Player::Draw() {
    auto& rm = ResourceManager::Get();
    auto& playerTexture = rm.GetTexture("player.png");

    RRectangle source(
        0.f, 0.f,
        playerTexture.GetWidth() / 17.f, playerTexture.GetHeight()
    );

    RRectangle dest(this->x, this->y, 0.9f, 1.45f);

    playerTexture.Draw(source, dest);
}