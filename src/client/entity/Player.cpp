#include "Player.hpp"
#include <Debug.hpp>
#include <Block.hpp>
#include <World.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <Logger.hpp>

#define WALK_SPEED 150
#define JUMP_SPEED 200
#define G 400

Player::Player(World* world) : SimplePlayer::SimplePlayer(world) {
    m_header = Header::PLAYER;
    m_texture = LoadTexture("assets/player.png");
    m_camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    m_camera.zoom = 1.5f;
    m_camera.rotation = 0.0f;
    m_enabledPhysics = true;

    updateCamera();
}

Player::~Player() {
    UnloadTexture(m_texture);
}

void Player::updateCamera() {
    this->m_camera.target = {m_hitbox.x + m_hitbox.width / 2, m_hitbox.y - m_hitbox.height / 2};

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
                m_animCurrentFrame = animationClamp(m_animCurrentFrame, 1, 5);
                break;
            case PLAYER_SNEAK:
                m_animCurrentFrame = animationClamp(m_animCurrentFrame, 6, 7);
                break;
            case PLAYER_JUMP:
                m_animCurrentFrame = 8;
                break;
            case PLAYER_HIT:
                m_animCurrentFrame = animationClamp(m_animCurrentFrame, 9, 13);
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

Vector2 Player::convertToCameraPos(Vector2 pos) {
    return Vector2 {
        (m_camera.target.x - m_camera.offset.x / m_camera.zoom) + pos.x / m_camera.zoom, 
        (m_camera.target.y - m_camera.offset.y / m_camera.zoom) + pos.y / m_camera.zoom
    };
}

Vector2 Player::getTargetBlock(bool onlyExist) {
    auto cur = convertToCameraPos(GetMousePosition());
    auto targetBlock = Vector2 {floor(cur.x / BS), floor(cur.y / BS)};

    if (Vector2Distance(cur, {m_hitbox.x, m_hitbox.y}) / BS <= 4 && 
        (!onlyExist || m_world->getBlock(targetBlock.x, targetBlock.y, IsKeyPressed(KEY_LEFT_ALT) ? 0 : 1) != nullptr)) {
        return targetBlock;
    } else {
        return Vector2 {-1, -1};
    }
}

void Player::updateControls() {
    auto targetBlockPos = getTargetBlock(false);
    auto targetBlock = m_world->getBlock(targetBlockPos.x, targetBlockPos.y, !IsKeyDown(KEY_LEFT_ALT));
    Block::BlockType targetBlockType = ((targetBlock != nullptr) ? targetBlock->getType() : Block::BlockType::AIR);

    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && targetBlockPos.x >= 0 && targetBlockPos.y >= 0 && targetBlockType != Block::BlockType::BEDROCK && targetBlockType != Block::BlockType::AIR) {
        if(m_canJump) setAnimation(PLAYER_HIT);
        m_world->destroyBlock(targetBlockPos.x, targetBlockPos.y, !IsKeyDown(KEY_LEFT_ALT));
    }

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && 
        (IsKeyDown(KEY_LEFT_ALT) || !CheckCollisionRecs(m_hitbox.to<Rectangle>(), {targetBlockPos.x * BS, targetBlockPos.y * BS, BS, BS})) && 
        targetBlockPos.x >= 0 && targetBlockPos.y >= 0) {
            
        if(m_canJump) setAnimation(PLAYER_HIT);
        m_world->placeBlock(targetBlockPos.x, targetBlockPos.y, !IsKeyDown(KEY_LEFT_ALT), m_selectedBlock);
    }

    for(int i = 0; i < 6; i++) {
        if(IsKeyDown(KEY_ONE + i)) {
            m_selectedBlock = (Block::BlockType)(i + 1);
        }
    }

    if(IsKeyDown(KEY_D)) {
        m_direction = -1;
        m_speed.x = WALK_SPEED;
    }

    if(IsKeyDown(KEY_A)) {
        m_direction = 1;
        m_speed.x = -WALK_SPEED;
    }

    if((IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE)) && (m_canJump || m_fly)) {
        m_speed.y = -JUMP_SPEED;
        m_canJump = false;
    }

    if(IsKeyDown(KEY_S) && m_fly && !m_canJump) {
        m_speed.y = JUMP_SPEED;
    }

    if((IsKeyDown(KEY_S) || IsKeyDown(KEY_LEFT_SHIFT)) && !m_fly && m_canJump) {
        m_sneak = true;
        m_speed.x /= 2;
    }

    if(IsKeyDown(KEY_R)) {
        m_hitbox.x = (float)(rand() % m_world->getWidth() * BLOCK_SIZE_PIXELS);
        m_hitbox.y = 0.0f;
    }

    if(IsKeyPressed(KEY_F)) {
        m_fly = !m_fly;
    }
}

void Player::update() {
    if(m_fly) m_speed.y = 0;
    m_speed.x = 0;
    m_sneak = false;
    m_gravitation = (!m_fly) ? 400.f : 0.0f;
    m_animFps = 10;

    updateControls();
    Entity::update();

    auto hitLimit = m_animLimits.at(PLAYER_HIT);
    if(m_animCurrentFrame < hitLimit.first || m_animCurrentFrame >= hitLimit.second) {
        setAnimation(PLAYER_IDLE);
    }

    if(m_sneak && m_speed.x == 0) {
        setAnimation(PLAYER_SNEAK);
        m_animFps = 0;
    }

    if(m_speed.x != 0 && m_canJump) {
        setAnimation((m_sneak) ? PLAYER_SNEAK : PLAYER_MOVE);
    }

    if(!m_canJump) {
        setAnimation(PLAYER_JUMP);
    }

    updateCamera();
    updateAnimation();
    
    if(Game::get()->isMultiplayer()) {
        updateMultiplayer();
    }

    Debug::addString(TextFormat("Player position: [%.0f, %.0f]", this->m_hitbox.x, this->m_hitbox.y));
    Debug::addString(TextFormat("Player speed: [%.0f, %.0f]", this->m_speed.x, this->m_speed.y));
    Debug::addString(TextFormat("Fly: %d", m_fly));
    Debug::addString(TextFormat("My playerID: %d", m_id));
}

void Player::updateMultiplayer() {
    static auto multiplayer = Game::get()->getMultiplayer();

    if(m_lastHitbox.x != m_hitbox.x || m_lastHitbox.y != m_hitbox.y) {
        multiplayer->addToQueue(new GamePacket(SerializedObject::PLAYER_POSITION, Vec2f {m_hitbox.x, m_hitbox.y}));
        m_lastHitbox = m_hitbox;
    }

    if(m_lastCurrentFrame != m_animCurrentFrame) {
        multiplayer->addToQueue(new GamePacket(SerializedObject::PLAYER_ANIMATION, m_animCurrentFrame));
        m_lastCurrentFrame = m_animCurrentFrame;
    }

    if (m_lastDirection != m_direction) {
        multiplayer->addToQueue(new GamePacket(SerializedObject::PLAYER_DIRECTION, m_direction));
        m_lastDirection = m_direction;
    }
}

void Player::processPhysics(bool hitWall, bool hitFloor, bool hitCeil) {
    m_canJump = hitFloor;

    Debug::addString(TextFormat("Hit floor: %d", hitFloor));
    Debug::addString(TextFormat("Hit wall: %d", hitWall));
}

void Player::draw() {
    float frameWidth = m_texture.width / 17;
    Rectangle src = {m_animCurrentFrame * frameWidth, 0, frameWidth * m_direction, (float)m_texture.height};
    Rectangle dest = {m_hitbox.x + m_hitbox.width / 2 - frameWidth, m_hitbox.y + m_hitbox.height - m_texture.height * 2, frameWidth * 2, (float)m_texture.height * 2};
    DrawTexturePro(m_texture, src, dest, {0, 0}, 0, WHITE);

    if(Debug::m_debug) {
        // for(auto& hitbox : m_hitboxes) {
        //     DrawRectangleLinesEx(hitbox, 1.0f, RED);
        // }

        DrawRectangleLinesEx(m_hitbox.to<Rectangle>(), 1.0f, GREEN);
    }
}

bool Player::isChunkInView(Chunk* chunk) {
    auto minPos = chunk->getPosition() * CHUNK_WIDTH * BS;
    auto maxPos = minPos + CHUNK_WIDTH * BS;
    auto min = convertToCameraPos({0, 0});
    auto max = convertToCameraPos({(float)GetScreenWidth(), (float)GetScreenHeight()});

    return (minPos >= min.x && minPos <= max.x) || (maxPos >= min.x && maxPos <= max.x); 
}