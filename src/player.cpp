#include <player.hpp>
#include <Debug.hpp>
#include "Block.hpp"
#include <GenericTools.hpp>


#define DELTA 0.0041f
#define G 9.8f

Player::Player() {
    m_objectID = 1;
    this->m_texture = LoadTexture("assets/player.png");
    this->m_rect = {0, 1000, 32, 64};
    this->updateCamera();
}

void Player::updateCamera() {
    this->camera.target = {m_rect.x + m_rect.width / 2, m_rect.y - m_rect.height / 2};
    this->camera.zoom = 1.5f;
    this->camera.rotation = 0.0f;

    Debug::addString(TextFormat("Camera target: [%.0f, %.0f]", this->camera.target.x, this->camera.target.y));
    Debug::addString(TextFormat("Camera zoom: %.02f", this->camera.zoom));
    Debug::addString(TextFormat("Camera rotation: %.02f", this->camera.rotation));
}

void Player::update(std::vector<Vector2> envHitboxes) {
    if (IsKeyDown(KEY_D)) {
        m_rect.x += m_speed;
    }

    if (IsKeyDown(KEY_A)) {
        m_rect.x -= m_speed;
    }

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE)) {
        // m_speed = -10.f;
        // m_canJump = false;
        m_rect.y -= m_speed;
    }

    if (IsKeyDown(KEY_S)) {
        m_rect.y += m_speed;
    }

    // bool hitObstacle = false;

    // for(auto& hitbox : envHitboxes) {
    //     if(CheckCollisionRecs(m_rect, {hitbox.x, hitbox.y, BLOCK_SIZE_PIXELS, BLOCK_SIZE_PIXELS})) {
    //         hitObstacle = true;
    //         m_speed = 0.0f;
    //         break;
    //     }
    // }

    // if (!hitObstacle) {
    //     m_rect.y += m_speed;
    //     m_speed += G;
    //     m_canJump = false;
    // } else { 
    //     m_canJump = true;
    // }

    this->updateCamera();

    Debug::addString(TextFormat("Player position: [%.0f, %.0f]", this->m_rect.x, this->m_rect.y));
}

void Player::draw() {
    DrawTexturePro(
        this->m_texture, 
        {0, 0, (float)m_texture.width, (float)m_texture.height}, 
        {m_rect.x, m_rect.y, (float)m_texture.width, (float)m_texture.height},
        {(float)m_texture.width / 2, (float)m_texture.height}, 0, WHITE
    );
}

Player::SObject Player::encodeObject() {
    SObject obj = SerializedObject::encodeObject();

    GenericTools::addVectors(
        &obj,
        GenericTools::valueToVector<float>(&m_rect.x)
    );
    GenericTools::addVectors(
        &obj,
        GenericTools::valueToVector<float>(&m_rect.y)
    );

    return obj;
};
int Player::decodeObject(SObject &s) {
    unsigned int _offset = SerializedObject::decodeObject(s);
    unsigned int offset = _offset;
    unsigned int required = sizeof(float) * 2;
    
    if (required > s.size() - offset) return s.size() - offset;

    m_rect.x = GenericTools::vectorToValue<float>(s, offset);
    offset += sizeof(float);

    m_rect.y = GenericTools::vectorToValue<float>(s, offset);
    offset += sizeof(float);

    return s.size();
}