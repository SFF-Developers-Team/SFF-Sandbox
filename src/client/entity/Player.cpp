#include "Player.hpp"
#include <Debug.hpp>
#include <Block.hpp>
#include <World.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <Logger.hpp>

#define WALK_SPEED 10

Player::Player(World* world) : SimplePlayer::SimplePlayer(world) {
    m_header = Header::PLAYER;
    m_texture = LoadTexture("assets/player.png");
    m_camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    m_camera.zoom = 50.0f;
    m_camera.rotation = 0.0f;

    updateCamera();
}

Player::~Player() {
    UnloadTexture(m_texture);
}

void Player::updateCamera() {
    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel > 0) {
        m_camera.zoom -= 1.0f;
    }
    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel < 0) {
        m_camera.zoom += 1.0f;
    }
    
    this->m_camera.target = {m_hitbox.x + m_hitbox.width / 2, m_hitbox.y - m_hitbox.height / 2};

    Debug::addString("Camera target: [{:0f}, {:0f}]", this->m_camera.target.x, this->m_camera.target.y);
    Debug::addString("Camera zoom: {:02f}", this->m_camera.zoom);
    Debug::addString("Camera rotation: {:02f}", this->m_camera.rotation);
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
    
    Debug::addString("Animation frame: {}", m_animCurrentFrame);
    Debug::addString("Animation FPS: {}", m_animFps);
    Debug::addString("Animation type: {}", getAnimationName(m_animType));
    Debug::addString("Animation last frame: {}", m_animLastFrameTime);
    Debug::addString("GetTime(): {}", GetTime());
}

Vector2 Player::convertToCameraPos(Vector2 pos) {
    return Vector2 {
        (m_camera.target.x - m_camera.offset.x / m_camera.zoom) + pos.x / m_camera.zoom, 
        (m_camera.target.y - m_camera.offset.y / m_camera.zoom) + pos.y / m_camera.zoom
    };
}

Vec2i Player::getTargetBlock(bool onlyExist) {
    auto cur = convertToCameraPos(GetMousePosition());
    auto targetBlock = Vec2i {(int)(cur.x), (int)(cur.y)};

    if (Vector2Distance(cur, {m_hitbox.x, m_hitbox.y}) <= 4 && 
        (!onlyExist || m_world->getBlock(targetBlock.x, targetBlock.y, IsKeyPressed(KEY_LEFT_ALT) ? 0 : 1) != nullptr)) {
        return targetBlock;
    } else {
        return Vec2i {-1, -1};
    }
}

bool Player::canDestroyBlock(Vec2i pos, uint8_t layer) {
    if(m_world->isOutOfBound(pos.x, pos.y, layer)) return false;

    auto block = m_world->getBlock(pos.x, pos.y, layer);
    if(!block) return false;

    Block::BlockType type = block->getType();
    
    return type != Block::BlockType::AIR; //type != Block::BlockType::BEDROCK &&
}

bool Player::canPlaceBlock(Vec2i pos, uint8_t layer) {
    if(m_world->isOutOfBound(pos.x, pos.y, layer)) return false;

    auto block = m_world->getBlock(pos.x, pos.y, layer);
    if(!block) {
        m_world->setBlock(pos.x, pos.y, layer, std::make_unique<Block>(Block::BlockType::AIR));
    }

    if(layer == 1 && CheckCollisionRecs(m_hitbox.getRect().to<Rectangle>(), {(float)pos.x, (float)pos.y, 1.0f, 1.0f})) return false;
    if (m_world->getBlock(pos.x - 1, pos.y, layer)->getType() == Block::BlockType::AIR &&
        m_world->getBlock(pos.x + 1, pos.y, layer)->getType() == Block::BlockType::AIR &&
        m_world->getBlock(pos.x, pos.y - 1, layer)->getType() == Block::BlockType::AIR &&
        m_world->getBlock(pos.x, pos.y + 1, layer)->getType() == Block::BlockType::AIR && 
        m_world->getBlock(pos.x, pos.y, !layer)->getType() == Block::BlockType::AIR
    ) return false;
    return m_world->getBlock(pos.x, pos.y, layer)->getType() == Block::BlockType::AIR;
}

void Player::updateControls() {
    auto targetBlockPos = getTargetBlock(false);
    auto gravitation = (!m_fly) ? 0.02f : 0.0f;
    auto isAltLayer = IsKeyDown(KEY_LEFT_ALT);
    auto forward = 0.0f;
    auto mp = Game::get()->getMultiplayer();

    m_sneak = false;

    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && canDestroyBlock(targetBlockPos, !isAltLayer)) {
        m_world->destroyBlock(targetBlockPos.x, targetBlockPos.y, !isAltLayer);
        if(m_onGround) setAnimation(PLAYER_HIT);
        if(Game::get()->isMultiplayer()) mp->onBlockChanged(targetBlockPos, !isAltLayer);
    }

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && canPlaceBlock(targetBlockPos, !isAltLayer)) {
        if(m_onGround) setAnimation(PLAYER_HIT);
        m_world->placeBlock(targetBlockPos.x, targetBlockPos.y, !IsKeyDown(KEY_LEFT_ALT), m_selectedBlock);
        if(Game::get()->isMultiplayer()) mp->onBlockChanged(targetBlockPos, !isAltLayer);
    }

    if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        m_direction = RIGHT;
        forward++;
    }

    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        m_direction = LEFT;
        forward--;
    }

    if((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE)) && (m_onGround || m_fly)) {
        m_speedY = ((!m_fly) ? -0.3f : -0.25f);
    }

    if((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && m_fly && !m_onGround) {
        m_speedY = 0.25f;
    }

    if((IsKeyDown(KEY_S) || IsKeyDown(KEY_LEFT_SHIFT)) && !m_fly && m_onGround) {
        m_sneak = true;
    }

    moveRelative(forward, (m_onGround ? (m_sneak ? 0.05f : 0.12f) : 0.1f));

    m_speedY += gravitation;

    move(m_speedX, m_speedY);

    m_speedX *= 0.91f;
    m_speedY *= 0.98f;

    if (m_onGround) {
        m_speedX *= 0.8f;
        m_speedY *= 0.8f;
    }

    if(m_fly) {
        m_speedY *= 0.7f;
    }
}

void Player::onTick() {
    Entity::onTick();
    m_animFps = 10;

    updateControls();

    auto hitLimit = m_animLimits.at(PLAYER_HIT);
    if(m_animCurrentFrame < hitLimit.first || m_animCurrentFrame >= hitLimit.second) {
        setAnimation(PLAYER_IDLE);
    }

    if(m_sneak) {
        setAnimation(PLAYER_SNEAK);
        m_animFps = 0;
    }

    if(m_onGround && (m_speedX > 0.025f || m_speedX < -0.025f)) {
        m_animFps = (m_sneak ? 7 : 10);
        setAnimation((m_sneak ? PLAYER_SNEAK : PLAYER_MOVE));
    }

    if(!m_onGround) {
        setAnimation(PLAYER_JUMP);
    }

    

    Debug::addString("Player position: [{:0f}, {:0f}]", this->m_hitbox.x, this->m_hitbox.y);
    Debug::addString("Player speed: [{:0f}, {:0f}]", this->m_speedX, this->m_speedY);
    Debug::addString("Fly: {}", m_fly);
    Debug::addString("OnGround: {}", m_onGround);
    Debug::addString("My playerID: {}", m_id);
}

void Player::update() {
    wheel = GetMouseWheelMove();
    for(int i = 0; i < 6; i++) {
        if(IsKeyDown(KEY_ONE + i)) {
            m_selectedBlock = (Block::BlockType)(i + 1);
        }
    }
    if (!IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.0f) {
        if((int)m_selectedBlock >= 6) {
            m_selectedBlock = (Block::BlockType)((int)(m_selectedBlock) - 6);
        }
        m_selectedBlock = (Block::BlockType)((int)(m_selectedBlock) + 1);
    }
    if(IsKeyPressed(KEY_R)) resetPosition();
    if(IsKeyPressed(KEY_F)) m_fly = !m_fly;

    updateCamera();
    updateAnimation();
}

void Player::moveCameraRelative(float x, float y) {
    float xa = m_camera.target.x - x;
    float ya = m_camera.target.y - y;

    auto dist = xa + ya;
    if (dist >= .01f) {
        dist = 0.5f / sqrtf(dist);
        xa *= dist;
        ya *= dist;
        m_camera.target.x += xa;
        m_camera.target.y += ya;
    }
}

bool Player::isChunkInView(Chunk* chunk) {
    auto minPos = chunk->getPosition() * CHUNK_WIDTH;
    auto maxPos = minPos + CHUNK_WIDTH;
    auto min = convertToCameraPos({0, 0});
    auto max = convertToCameraPos({(float)GetScreenWidth(), (float)GetScreenHeight()});

    return (minPos >= min.x && minPos <= max.x) || (maxPos >= min.x && maxPos <= max.x); 
}