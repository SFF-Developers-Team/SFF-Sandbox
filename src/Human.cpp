#include <Human.hpp>
#include <Debug.hpp>
#include <Block.hpp>
#include <World.hpp>
#include <Game.hpp>
#include <algorithm>
#include <Chunk.hpp>

#define WALK_SPEED 150
#define JUMP_SPEED 200
#define G 400

Human::Human(World* world) : Entity::Entity(world) {
    m_header = Header::PLAYER;

    auto game = Game::get();

    if(m_texture.width == 0) {
        m_texture = LoadTexture("assets/player.png");
    }

    m_hitbox = {(float)(rand() % m_world->getWidth() * BS), 0, 25.f, 44.f};
    m_enabledPhysics = true;
    m_gravitation = 400.f;
}

Human::~Human() {
    UnloadTexture(m_texture);
}

void Human::updateAnimation() {
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
}

// for debug
const char* Human::getAnimationName(AnimationType type) {
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

void Human::setAnimation(AnimationType type) {
    m_animType = type;

    if(m_animLimits.count(type)) {
        auto limit = m_animLimits.at(type);
        m_animCurrentFrame = animationClamp(m_animCurrentFrame, limit.first, limit.second);
    }
}

unsigned char Human::animationClamp(unsigned char value, unsigned char min, unsigned char max) {
    if(value > max) return min;
    if(value < min) return min;
    return value;
}

void Human::update() {
    m_speed.x = WALK_SPEED * -m_direction;

    if(GetTime() >= m_lastActionTime + 0.5f) {
        m_lastActionTime = GetTime();

        m_direction = (rand() % 2 == 0) ? -1 : 1;
        
        if(rand() % 2 == 1 && m_canJump) {
            m_speed.y = -JUMP_SPEED;
        }
    }

    setAnimation(PLAYER_IDLE);
    Entity::update();

    if(m_speed.x != 0 && m_canJump) {
        setAnimation(PLAYER_MOVE);
    }

    updateAnimation();
}

void Human::processPhysics(bool hitWall, bool hitFloor, bool hitCeil) {
    if(!hitFloor) {
        setAnimation(PLAYER_JUMP);
        m_canJump = false;
    } else {
        m_canJump = true;
    }
}

void Human::draw() {
    float frameWidth = m_texture.width / 17;
    Rectangle src = {m_animCurrentFrame * frameWidth, 0, frameWidth * m_direction, (float)m_texture.height};
    Rectangle dest = {m_hitbox.x + m_hitbox.width / 2 - frameWidth, m_hitbox.y + m_hitbox.height - m_texture.height * 2, frameWidth * 2, (float)m_texture.height * 2};
    DrawTexturePro(m_texture, src, dest, {0, 0}, 0, WHITE);

    if(Debug::m_debug) {
        // for(auto& hitbox : m_hitboxes) {
        //     DrawRectangleLinesEx(hitbox, 1.0f, RED);
        // }

        DrawRectangleLinesEx(m_hitbox, 1.0f, GREEN);
    }
}