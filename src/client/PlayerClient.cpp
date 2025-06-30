#include "PlayerClient.hpp"
#include "AnimatedTexture.hpp"
#include "Types.hpp"
#include "World.hpp"
#include "Debug.hpp"
#include <format>

int playerFrames[] = {0, 1, 6, 8, 9, 14, 15, 16};

PlayerClient::PlayerClient(World& world) : Entity(world), AnimatedTexture("player.png", playerFrames, 17) {
    SetSize(PLAYER_BOX_WIDTH, PLAYER_BOX_HEIGHT);
    SetDirection(DIRECTION_LEFT);
}

void PlayerClient::OnTick() {
    if (raylib::Keyboard::IsKeyDown(KEY_A)) {
        m_speed.SetX(-0.2f);
        SetDirection(DIRECTION_LEFT);
    }

    if (raylib::Keyboard::IsKeyDown(KEY_D)) {
        m_speed.SetX(0.2f);
        SetDirection(DIRECTION_RIGHT);
    }

    if ((raylib::Keyboard::IsKeyDown(KEY_W) || raylib::Keyboard::IsKeyDown(KEY_SPACE)) && m_onGround) {
        m_speed.SetY(-0.3f);
    }
    if (raylib::Keyboard::IsKeyDown(KEY_R)) {
        this->x = 0;
        this->y = 286;
    }

    Entity::OnTick();
    if (m_speed.x != 0.f) {
        SetType(PLAYER_ANIMATION_TYPE_WALK);
    } 
    
    // too slow
    if (m_speed.x > -0.01f && m_speed.x < 0.01f) {
        SetType(PLAYER_ANIMATION_TYPE_IDLE);
    }

    if (!m_onGround) {
        SetType(PLAYER_ANIMATION_TYPE_JUMP, true);
    }

    AnimatedTexture::OnTick();
}

void PlayerClient::Draw(raylib::Window& window) {
    RRectangle dest(this->x, this->y, PLAYER_BOX_WIDTH, PLAYER_BOX_HEIGHT);

    AnimatedTexture::Draw(dest);
}