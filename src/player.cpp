#include <Player.hpp>

Player::Player() {
    this->m_texture = LoadTexture("assets/player.png");
    this->m_rect = {0, 1000, 32, 64};
    this->updateCamera();
}

void Player::updateCamera() {
    this->camera.target = {m_rect.x + m_rect.width / 2, m_rect.y - m_rect.height / 2};
    this->camera.zoom = 1.5f;
    this->camera.rotation = 0.0f;
}

void Player::update() {
    if (IsKeyDown(KEY_D)) {
        m_rect.x += playerSpeed;
    }

    if (IsKeyDown(KEY_A)) {
        m_rect.x -= playerSpeed;
    }

    if (IsKeyDown(KEY_W)) {
        m_rect.y -= playerSpeed;
    }

    if (IsKeyDown(KEY_S)) {
        m_rect.y += playerSpeed;
    }

    this->updateCamera();  
}

void Player::draw() {
    DrawTexturePro(
        this->m_texture, 
        {0, 0, (float)m_texture.width, (float)m_texture.height}, 
        {m_rect.x, m_rect.y, (float)m_texture.width, (float)m_texture.height},
        {(float)m_texture.width / 2, (float)m_texture.height}, 0, WHITE
    );
}