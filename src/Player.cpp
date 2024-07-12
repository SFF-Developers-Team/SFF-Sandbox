#include <Player.hpp>
#include <Debug.hpp>
#include <Block.hpp>
#include <GenericTools.hpp>
#include <World.hpp>
#include <Game.hpp>
#include <algorithm>

#define WALK_SPEED 200
#define JUMP_SPEED 200
#define G 400

Player::Player(World* world) {
    m_world = world;
    m_objectID = 1;

    auto game = Game::get();

    m_texture = LoadTexture("assets/player.png");
    m_hitbox = {(float)(rand() % m_world->getWidth() * BS), 0, 30.f, 44.f};
    m_camera.offset = {game->getScreenWidth() / 2.0f, game->getScreenHeight() / 2.0f};

    updateCamera();
}

Player::~Player() {
    UnloadTexture(m_texture);
}

void Player::updateCamera() {
    this->m_camera.target = {m_hitbox.x + m_hitbox.width / 2, m_hitbox.y - m_hitbox.height / 2};
    this->m_camera.zoom = 1.5f;
    this->m_camera.rotation = 0.0f;

    Debug::addString(TextFormat("Camera target: [%.0f, %.0f]", this->m_camera.target.x, this->m_camera.target.y));
    Debug::addString(TextFormat("Camera zoom: %.02f", this->m_camera.zoom));
    Debug::addString(TextFormat("Camera rotation: %.02f", this->m_camera.rotation));
}

void Player::updateAnimation() {
    if(GetTime() >= m_animLastFrameTime + (1.0f / m_animFps)) {
        m_animLastFrameTime = GetTime();
        m_animCurrentFrame++;

        switch(m_animType) {
            case PLAYER_IDLE:
                m_animCurrentFrame = 0;
                break;
            case PLAYER_MOVE:
                if(m_animCurrentFrame > 5) m_animCurrentFrame = 1;
                break;
            case PLAYER_SNEAK:
                if(m_animCurrentFrame > 7) m_animCurrentFrame = 6;
                break;
            case PLAYER_JUMP:
                m_animCurrentFrame = 8;
                break;
            case PLAYER_HIT:
                if(m_animCurrentFrame > 13) m_animCurrentFrame = 9;
                break;
            case PLAYER_HURT:
                m_animCurrentFrame = 14;
                break;
            case PLAYER_SIT:
                m_animCurrentFrame = 15;
                break;
            case PLAYER_CART:
                m_animCurrentFrame = 16;
                break;
        }
    }

    Debug::addString(TextFormat("Animation frame: %d", m_animCurrentFrame));
    Debug::addString(TextFormat("Animation FPS: %d", m_animFps));
    Debug::addString(TextFormat("Animation type: %s", getAnimationName(m_animType)));
    Debug::addString(TextFormat("Animation last frame: %f", m_animLastFrameTime));
    Debug::addString(TextFormat("GetTime(): %f", GetTime()));
}

// for debug
const char* Player::getAnimationName(AnimationType type) {
    switch(type) {
        case PLAYER_IDLE:
            return "PLAYER_IDLE";
        case PLAYER_MOVE:
            return "PLAYER_MOVE";
        case PLAYER_SNEAK:
            return "PLAYER_SNEAK";
        case PLAYER_JUMP:
            return "PLAYER_JUMP";
        case PLAYER_HIT:
            return "PLAYER_HIT";
        case PLAYER_HURT:
            return "PLAYER_HURT";
        case PLAYER_SIT:
            return "PLAYER_SIT";
        case PLAYER_CART:
            return "PLAYER_CART";
    }
}

void Player::setAnimation(AnimationType type) {
    m_animType = type;
    // m_animLastFrameTime = GetTime();
}

Vector2 Player::convertToCameraPos(Vector2 pos) {
    return Vector2 {
        (m_camera.target.x - m_camera.offset.x / m_camera.zoom) + pos.x / m_camera.zoom, 
        (m_camera.target.y - m_camera.offset.y / m_camera.zoom) + pos.y / m_camera.zoom
    };
}

Vector2 Player::getTargetBlock() {
    auto cur = convertToCameraPos(GetMousePosition());

    if(Vector2Distance(cur, {m_hitbox.x, m_hitbox.y}) / BS <= 4) {
        return Vector2 {floor(cur.x / BS), floor(cur.y / BS)};
    } else {
        return Vector2 {-1, -1};
    }
}

void Player::update(std::vector<Rectangle> envHitboxes) {
    auto cursor = GetMousePosition();
    auto delta = GetFrameTime();
    auto targetBlock = getTargetBlock();
    bool hitFloor = false;
    bool hitWall = false;
    bool hitCeil = false;

    m_speed.x = 0;
    setAnimation(PLAYER_IDLE);

    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && targetBlock.x >= 0 && targetBlock.y >= 0) {
        setAnimation(PLAYER_HIT);
        m_world->destroyBlock(targetBlock.x, targetBlock.y);
    }

    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && 
        !CheckCollisionRecs(m_hitbox, {targetBlock.x * BS, targetBlock.y * BS, BS, BS}) && 
        targetBlock.x >= 0 && targetBlock.y >= 0) {
        m_world->placeBlock(targetBlock.x, targetBlock.y, Block::BlockType::STONE);
    }

    if(IsKeyDown(KEY_D)) {
        m_direction = -1;
        m_speed.x = WALK_SPEED;
    }

    if(IsKeyDown(KEY_A)) {
        m_direction = 1;
        m_speed.x = -WALK_SPEED;
    }

    if(IsKeyDown(KEY_W) && (m_canJump || m_fly)) {
        m_speed.y = -JUMP_SPEED;
        m_canJump = false;
    }

    if(IsKeyDown(KEY_S) && m_fly && !hitFloor) {
        m_hitbox.y += WALK_SPEED * delta;
    }

    if(IsKeyDown(KEY_R)) {
        m_hitbox.x = (float)(rand() % 256 * BLOCK_SIZE_PIXELS);
        m_hitbox.y = 0.0f;
    }

    if(IsKeyPressed(KEY_F)) {
        m_fly = !m_fly;
    }

    for(auto& bh : envHitboxes) {
        // hit right wall
        if (((bh.y > m_hitbox.y && bh.y < m_hitbox.y + m_hitbox.height) ||
            (bh.y + bh.height > m_hitbox.y && bh.y + bh.height < m_hitbox.y + m_hitbox.height)) &&
            m_hitbox.x + m_hitbox.width + m_speed.x * delta >= bh.x &&
            m_hitbox.x + m_hitbox.width <= bh.x + bh.width

        ) {
            m_hitbox.x = bh.x - m_hitbox.width;          
            m_speed.x = 0.0f;
            hitWall = true;
        }

        // hit left wall
        if (((bh.y > m_hitbox.y && bh.y < m_hitbox.y + m_hitbox.height) ||
            (bh.y + bh.height > m_hitbox.y && bh.y + bh.height < m_hitbox.y + m_hitbox.height)) &&
            m_hitbox.x + m_speed.x * delta <= bh.x + bh.width &&
            m_hitbox.x >= bh.x

        ) {
            m_hitbox.x = bh.x + bh.width;          
            m_speed.x = 0.0f;
            hitWall = true;
        }

        // hit the ceil
        if((bh.x < m_hitbox.x || bh.x < m_hitbox.x + m_hitbox.width) && 
            bh.x + bh.width > m_hitbox.x && 
            bh.y + bh.height <= m_hitbox.y && 
            bh.y + bh.height >= m_hitbox.y + m_speed.y * delta
        ) {
            m_hitbox.y = bh.y + bh.height;
            m_speed.y = abs(m_speed.y / 2);
            hitCeil = true;
        }

        // hit the floor
        if((bh.x < m_hitbox.x || bh.x < m_hitbox.x + m_hitbox.width) && 
            bh.x + bh.width > m_hitbox.x && 
            m_hitbox.y + m_hitbox.height <= bh.y && 
            m_hitbox.y + m_hitbox.height + m_speed.y * delta >= bh.y
        ) {
            m_hitbox.y = bh.y - m_hitbox.height;
            m_speed.y = 0.0f;
            hitFloor = true;
        }
    }

    if(!hitWall) {
        m_hitbox.x += m_speed.x * delta;
    }

    if(m_speed.x != 0) {
        setAnimation(PLAYER_MOVE);
    }

    if(!hitFloor) {
        m_hitbox.y += m_speed.y * delta;
        m_speed.y += (!m_fly) ? G * delta : 0;
        setAnimation(PLAYER_JUMP);
        m_canJump = false;
    } else {
        m_canJump = true;
    }
    
    updateCamera();
    updateAnimation();

    Debug::addString(TextFormat("Player position: [%.0f, %.0f]", this->m_hitbox.x, this->m_hitbox.y));
    Debug::addString(TextFormat("Player speed: [%.0f, %.0f]", this->m_speed.x, this->m_hitbox.y));
    Debug::addString(TextFormat("Hit floor: %d", hitFloor));
    Debug::addString(TextFormat("Hit wall: %d", hitWall));
    Debug::addString(TextFormat("Fly: %d", m_fly));
}

void Player::draw() {
    float frameWidth = m_texture.width / 17;
    Rectangle src = {m_animCurrentFrame * frameWidth, 0, frameWidth * m_direction, (float)m_texture.height};
    Rectangle dest = {m_hitbox.x, m_hitbox.y + m_hitbox.height - m_texture.height * 2, frameWidth * 2, (float)m_texture.height * 2};
    DrawTexturePro(m_texture, src, dest, {0, 0}, 0, WHITE);

    if(Debug::m_debug) {
        DrawRectangleLinesEx(m_hitbox, 1.0f, GREEN);
    }
}

Player::SObject Player::encodeObject() {
    SObject obj = SerializedObject::encodeObject();

    GenericTools::addVectors(
        &obj,
        GenericTools::valueToVector<float>(&m_hitbox.x)
    );
    GenericTools::addVectors(
        &obj,
        GenericTools::valueToVector<float>(&m_hitbox.y)
    );

    return obj;
}

int Player::decodeObject(SObject &s) {
    unsigned int _offset = SerializedObject::decodeObject(s);
    unsigned int offset = _offset;
    unsigned int required = sizeof(float) * 2;
    
    if (required > s.size() - offset) return s.size() - offset;

    m_hitbox.x = GenericTools::vectorToValue<float>(s, offset);
    offset += sizeof(float);

    m_hitbox.y = GenericTools::vectorToValue<float>(s, offset);
    offset += sizeof(float);

    return s.size();
}