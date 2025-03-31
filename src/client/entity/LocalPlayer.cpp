#include "raylib.h"
#include <managers/SettingsManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/Block.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <net/Multiplayer.hpp>
#include <managers/Debug.hpp>
#include <Utils.hpp>
#include <Game.hpp>
#include <algorithm>
#include <Types.hpp>
#include <ItemDatabase.hpp>
#include <raymath.h>

LocalPlayer::LocalPlayer(std::shared_ptr<World> world) : ClientPlayer(world), m_forward(0.f), m_camera{0} {
    m_camera.zoom = 50.f;
    m_camera.offset = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
}

void LocalPlayer::updateCamera() {
    float wheel = GetMouseWheelMove();
    Vector2 playerCenter = {
        m_hitbox.x + m_hitbox.width / 2,
        m_hitbox.y - m_hitbox.height / 2
    };

    m_camera.target = playerCenter;
    
    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0) {
        m_camera.zoom -= wheel;

        float scaleX = static_cast<float>(GetScreenWidth()) / 640;
        float scaleY = static_cast<float>(GetScreenHeight()) / 360;

        m_camera.zoom = std::clamp(m_camera.zoom, 5.f * scaleX, 90.f * scaleX);
    }

    if (IsWindowResized()) {
        m_camera.offset = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
    }
}

void LocalPlayer::updateAnimation() {
    float static lastAnimTime = 0.f;

    if (GetTime() >= lastAnimTime + (1.0f / m_animFps)) {
        lastAnimTime = GetTime();
        m_animFrame++;

        // clang-format: off
        switch (m_animType) {
            case PLAYER_IDLE: m_animFrame = 0; break;
            case PLAYER_MOVE: m_animFrame = animationClamp(m_animFrame, 1, 5); break;
            case PLAYER_SNEAK: m_animFrame = animationClamp(m_animFrame, 6, 7); break;
            case PLAYER_JUMP: m_animFrame = 8; break;
            case PLAYER_HIT: m_animFrame = animationClamp(m_animFrame, 9, 13); break;
            case PLAYER_HURT: m_animFrame = 14; break;
            case PLAYER_SIT: m_animFrame = 15; break;
            case PLAYER_CART: m_animFrame = 16; break;
        }
        // clang-format: on
    }
}

BlockPosition LocalPlayer::getTargetBlock() {
    Vector2 cur = TO_CAMERA_POS(m_camera, GetMousePosition());
    auto layer = !IsKeyDown(KEY_LEFT_ALT);

    return {static_cast<int>(floorl(cur.x)), static_cast<int>(floorl(cur.y)), layer};
}

void LocalPlayer::onTickControls(World* world) {
    auto target = getTargetBlock();
    auto mp = Multiplayer::get();
    auto ct = Game::get()->getControlType();

    if (canAccessBlock(target)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && canDestroyBlock(world, target)) {
            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            destroyBlock(world);
        }

        if (m_breakingBlock && IsMouseButtonUp(MOUSE_LEFT_BUTTON)) {
            world->stopBreakingBlock(m_id);
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && canPlaceBlock(world, target)) {
            if(IsKeyDown(KEY_LEFT_CONTROL)) {
                destroyBlock(world);
            }

            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            placeBlock(world);
        }
    }
}

void LocalPlayer::onTick(World* world) {
    Player::onTick(world);

    auto static prevAnimType = m_animType;
    auto static prevAnimFrame = m_animFrame;
    auto static prevDir = m_direction;
    m_animFps = 10;

    onTickControls(world);

    m_speed.x += (m_onGround ? (m_sneak ? 0.0025f : 0.06f) : 0.02f) * m_forward;
    m_speed.y += (!m_fly) ? 0.02f : 0.0f;

    auto hitboxesAround = world->getHitboxes(m_hitbox.expand(m_speed.x, m_speed.y));
    move(hitboxesAround, m_speed.x, m_speed.y);

    m_speed.x *= 0.91f;
    m_speed.y *= 0.98f;

    if (m_onGround) {
        m_speed.x *= 0.7f;
        m_speed.y *= 0.7f;
    }

    if (m_fly) {
        m_speed.y *= 0.7f;
    }

    m_forward = 0.f;

    auto hitLimit = getAnimLimit(PLAYER_HIT);
    if (m_animFrame < hitLimit.x || m_animFrame >= hitLimit.y) {
        setAnimation(PLAYER_IDLE);
    }

    if (m_onGround && (m_speed.x > 0.025f || m_speed.x < -0.025f)) {
        m_animFps = (m_sneak ? 7 : 10);
        setAnimation((m_sneak ? PLAYER_SNEAK : PLAYER_MOVE));
    }

    if (m_sneak) {
        setAnimation(PLAYER_SNEAK);
        m_animFps = (m_speed.x != 0.f ? 4 : 0);
    }

    if (!m_onGround) {
        setAnimation(PLAYER_JUMP);
    }

    if (world->getTime() < m_lastHurtTime + 15) {
        setAnimation(PLAYER_HURT);
    }

    auto mp = Multiplayer::get();

    if(!m_sneakToggled) {
        m_sneak = false;
    }

    auto static prevPos = getPosition();

    // clang-format off
    bool shouldupd = (
        prevPos.x != m_hitbox.x || 
        prevPos.y != m_hitbox.y || 
        prevAnimFrame != m_animFrame || 
        prevAnimType != m_animType || 
        prevDir != m_direction
    );
    // clang-format on

    if (mp->isConnected() && shouldupd) {
        mp->sendObj(std::shared_ptr<Player>(Game::get()->getPlayer()), EVERYTHING, false);
    }

    auto dbg = Debug::get();
    auto target = getTargetBlock();
    auto block = world->getBlock(target);

    dbg->setString(PLAYER_TARGET_BLOCK, "Target block: [{}, {}] ({})", target.x, target.y, (block ? block->getName() : "nullptr"));
    dbg->setString(PLAYER_POSITION, "Position: [{:.2f}, {:.2f}]", m_hitbox.x, m_hitbox.y);
    dbg->setString(PLAYER_HEALTH, "Health: {}", m_health);
}

void LocalPlayer::updateControls() {
    auto stm = SettingsManager::get();
    auto wheel = GetMouseWheelMove();

    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.f) {
        m_camera.zoom -= wheel * 10.f;
    }

    if(IsGestureDetected(GESTURE_PINCH_IN) || IsGestureDetected(GESTURE_PINCH_OUT)) {
        m_camera.zoom += GetGesturePinchAngle();
    }

    if (IsKeyPressed(stm->getKeybind("fly"))) {
        toggleFly();
    }

    if (IsKeyDown(stm->getKeybind("right")) || IsKeyDown(KEY_RIGHT)) {
        triggerMove(RIGHT);
    }

    if (IsKeyDown(stm->getKeybind("left")) || IsKeyDown(KEY_LEFT)) {
        triggerMove(LEFT);
    }

    if ((IsKeyDown(stm->getKeybind("jump")) || IsKeyDown(KEY_UP))) {
        triggerJump();
    }

    if ((IsKeyDown(stm->getKeybind("duck"))) || IsKeyDown(KEY_DOWN)) {
        triggerDuck(false);
    }

    if ((IsKeyReleased(stm->getKeybind("duck"))) || IsKeyReleased(KEY_DOWN)) {
        m_sneak = false;
    }

    if (IsKeyPressed(KEY_F4)) {
        m_gamemode = (m_gamemode == GAMEMODE_CREATIVE) ? GAMEMODE_SURVIVAL : GAMEMODE_CREATIVE;
    }
}

void LocalPlayer::update() {
    updateCamera();
    updateAnimation();
}

bool LocalPlayer::isChunkInView(Vec2i chunkPos) {
    Vec2i minPos = {chunkPos.x * CHUNK_WIDTH, chunkPos.y * CHUNK_HEIGHT};
    Vec2i maxPos = {minPos.x + CHUNK_WIDTH, minPos.y + CHUNK_HEIGHT};
    Vec2i zero = {0, 0};
    Vec2i screen = {GetScreenWidth(), GetScreenHeight()};
    Vector2 min = TO_CAMERA_POS(m_camera, zero);
    Vector2 max = TO_CAMERA_POS(m_camera, screen);

    bool isXIntersect = (min.x <= maxPos.x && max.x >= minPos.x);
    bool isYIntersect = (min.y <= maxPos.y && max.y >= minPos.y);

    return isXIntersect && isYIntersect;
}

bool LocalPlayer::isRectInView(Rectf rect) {
    Vec2i zero = {0, 0};
    Vec2i screen = {GetScreenWidth(), GetScreenHeight()};

    Vector2 min = TO_CAMERA_POS(m_camera, zero);
    Vector2 max = TO_CAMERA_POS(m_camera, screen);

    return (rect.x + rect.width >= min.x && rect.x <= max.x) && (rect.y + rect.height >= min.y && rect.y <= max.y);
}

void LocalPlayer::triggerMove(Direction dir) {
    m_direction = dir;
    (dir == RIGHT) ? m_forward++ : m_forward--;

    m_forward = std::clamp(m_forward, -1.f, 1.f);
}

void LocalPlayer::triggerJump() {
    if(m_onGround || m_fly) {
        m_speed.y = ((!m_fly) ? -0.3f : -0.25f);
    }
}

void LocalPlayer::triggerDuck(bool toggle) {
    if (m_fly && !m_onGround) {
        m_speed.y = 0.25f;
    }

    if(!m_fly && m_onGround) {
        m_sneak = true;
        
        if (toggle) {
            m_sneakToggled = !m_sneakToggled;
            m_sneak = m_sneakToggled;
        }
    }
}

void LocalPlayer::placeBlock(World* world) {
    if(getItem(m_selected) != nullptr) {
        auto target = getTargetBlock();
        auto mp = Multiplayer::get();

        world->placeBlock(m_id, target);

        // if (mp->isConnected()) {
        //     mp->sendPacket(Packet(ObjectHeader::BLOCK_PLACE, block->serialize()), BLOCKS);
        // }
    }
}

void LocalPlayer::destroyBlock(World* world) {
    auto mp = Multiplayer::get();
    auto target = getTargetBlock();

    world->breakBlock(m_id, target);

    if (mp->isConnected()) {
        mp->sendPacket(Packet(ObjectHeader::BLOCK_DESTROY, target), Channel::BLOCKS);
    }
}