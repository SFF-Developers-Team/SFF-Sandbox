#include "Player.hpp"
#include "World.hpp"
#include <raylib-cpp.hpp>
#include <raylib.h>

Player::Player(World& world) : Entity(world) {
    SetSize(0.9f, 1.45f);
}

void Player::Update() {
    if (raylib::Keyboard::IsKeyDown(KEY_A)) {
        m_speed.SetX(-0.2f);
    }

    if (raylib::Keyboard::IsKeyDown(KEY_D)) {
        m_speed.SetX(0.2f);
    }

    if ((raylib::Keyboard::IsKeyDown(KEY_W) || raylib::Keyboard::IsKeyDown(KEY_SPACE)) && m_onGround) {
        m_speed.SetY(-0.3f);
    }

    Entity::Update();
}