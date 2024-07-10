#include <player.hpp>
#include <Debug.hpp>
#include "Block.hpp"
#include <GenericTools.hpp>

#define WALK_SPEED 200
#define JUMP_SPEED 350
#define G 400
#define BS BLOCK_SIZE_PIXELS
#define WALL_PADDING 2
#define WP WALL_PADDING

Player::Player() {
    m_objectID = 1;
    this->m_texture = LoadTexture("assets/player.png");
    this->m_rect = {(float)(rand() % 256 * BLOCK_SIZE_PIXELS), 0.f, 32.f, 48.f};
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

void Player::update(std::vector<Rectangle> envHitboxes) {
    auto delta = GetFrameTime();

    m_speed.x = 0.0f;

    if (IsKeyDown(KEY_D)) {
        m_direction = 1;
        m_speed.x = WALK_SPEED;
    }

    if (IsKeyDown(KEY_A)) {
        m_direction = -1;
        m_speed.x = WALK_SPEED;
    }

    if (IsKeyDown(KEY_W) && (m_canJump || m_fly)) {
        if(m_fly) {
            m_rect.y -= WALK_SPEED * delta;
        } else {
            m_speed.y = -JUMP_SPEED;
            m_canJump = false;
        }
    }

    if(IsKeyDown(KEY_S) && m_fly) {
        m_rect.y += WALK_SPEED * delta;
    }

    if (IsKeyDown(KEY_R)) {
        m_rect.x = (float)(rand() % 256 * BLOCK_SIZE_PIXELS);
        m_rect.y = 0.0f;
    }

    if (IsKeyPressed(KEY_F)) {
        m_fly = !m_fly;
    }

    bool hitFloor = false;
    bool hitWall = false;

    for(auto& bh : envHitboxes) {
        // // hit right wall
        // if(bh.y >= m_rect.y && bh.y + bh.height <= m_rect.y + m_rect.height && m_rect.x + m_rect.width >= bh.x && m_rect.x + m_rect.width <= bh.x + bh.width) {
        //     m_rect.x = bh.x - m_rect.width;
        //     m_speed.x = 0.0f;
        //     hitWall = true;
        // }

        // // hit left wall
        // if(bh.y >= m_rect.y && bh.y + bh.height <= m_rect.y + m_rect.height && bh.x + bh.width <= m_rect.x) {
        //     m_rect.x = bh.x + bh.width;
        //     m_speed.x = 0.0f;
        //     hitWall = true;
        // }

        // // hit the ceil
        // if(CheckCollisionPointRec({m_rect.x, m_rect.y}, {bh.x * BS, bh.y * BS + (BS / 2), BS, BS / 2}) ||
        //    CheckCollisionPointRec({m_rect.x + m_rect.width, m_rect.y}, {bh.x * BS, bh.y * BS + (BS / 2), BS, BS / 2})) {
        //     m_rect.y = ((bh.y + 1) * BS);
        //     m_speed.y = 0.0f;
        // }

        // // hit the floor
        // if(CheckCollisionPointRec({m_rect.x, m_rect.y + m_rect.height}, {bh.x * BS, bh.y * BS, BS, BS / 2}) ||
        //    CheckCollisionPointRec({m_rect.x + m_rect.width, m_rect.y + m_rect.height}, {bh.x * BS, bh.y * BS, BS, BS / 2})) {
        //     m_rect.y = bh.y * BS - m_rect.height;
        //     m_speed.y = 0.0f;
        //     hitFloor = true;
        // }
    }

    if(!hitFloor && !m_fly) {
        m_rect.y += m_speed.y * delta;
        m_speed.y += G * delta;
        m_canJump = false;
    } else {
        m_canJump = true;
    }

    if(!hitWall) {
        m_rect.x += m_speed.x * m_direction * delta;
    }

    this->updateCamera();

    Debug::addString(TextFormat("Player position: [%.0f, %.0f]", this->m_rect.x, this->m_rect.y));
    Debug::addString(TextFormat("Hit floor: %d", hitFloor));
    Debug::addString(TextFormat("Hit wall: %d", hitWall));
    Debug::addString(TextFormat("Fly: %d", m_fly));
}

void Player::draw() {
    DrawTexturePro(
        this->m_texture, 
        {0, 0, (float)m_texture.width * m_direction, (float)m_texture.height}, 
        {m_rect.x, m_rect.y, (float)m_texture.width, (float)m_texture.height},
        {-m_rect.width / 2 + m_texture.width / 2, m_texture.height - m_rect.height}, 0, WHITE
    );

    if(Debug::m_debug) {
        DrawRectangleLinesEx(m_rect, 1.0f, GREEN);
    }
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