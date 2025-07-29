#include "PlayerClient.hpp"
#include "AnimatedTexture.hpp"
#include "Types.hpp"
#include "World.hpp"

int playerFrames[] = {0, 1, 6, 8, 9, 14, 15, 16};

PlayerClient::PlayerClient(World& world, RVector2 position, raylib::Camera2D camera) : Player(world, position), AnimatedTexture("player.png", playerFrames, 17), m_camera(camera) {
    SetSize(PLAYER_BOX_WIDTH * 0.9f, PLAYER_BOX_HEIGHT);
}

BlockPosition PlayerClient::GetTargetBlock() {   
    raylib::Vector2 const cursor = raylib::Mouse::GetPosition();

    // World float cursor
    raylib::Vector2 cursorWorld = m_camera.GetScreenToWorld(cursor);

    // World int cursor
    Vector2i cursorWorldI(static_cast<int>(std::floor(cursorWorld.x)), static_cast<int>(std::floor(cursorWorld.y)));

    int const selectedLayer = !raylib::Keyboard::IsKeyDown(KEY_LEFT_ALT);
    m_blockPos = {(float)cursorWorldI.x, (float)cursorWorldI.y, selectedLayer};

    return m_blockPos; 
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

    Entity::OnTick();

    if (m_speed.x != 0.f) {
        SetType(PLAYER_ANIMATION_TYPE_WALK);
    }

    // too slow
    if (m_speed.x > -0.01f && m_speed.x < 0.01f) {
        SetType(PLAYER_ANIMATION_TYPE_IDLE);
    }
    if (m_isDig) {
        SetType(PLAYER_ANIMATION_TYPE_HIT);
    }
    if (!m_onGround) {
        SetType(PLAYER_ANIMATION_TYPE_JUMP, true);
    }

    if (m_world.GetTicks() < GetLastTimeHurt() + 10) {
        SetType(PLAYER_ANIMATION_TYPE_HURT, true);
    }
    AnimatedTexture::OnTick();
}

void PlayerClient::Draw() {
    AnimatedTexture::Draw({this->x, this->y, PLAYER_BOX_WIDTH, PLAYER_BOX_HEIGHT});
}