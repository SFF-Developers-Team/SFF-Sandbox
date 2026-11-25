#include <RaylibExtras.hpp>
#include <raylib-cpp.hpp>
#include <managers/SettingsManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/Tile.hpp>
#include <world/World.hpp>
#include <net/Multiplayer.hpp>
#include <managers/Debug.hpp>
#include <Utils.hpp>
#include <Game.hpp>
#include <algorithm>
#include <Types.hpp>
#include <raymath.h>

LocalPlayer::LocalPlayer(World& world) : ClientPlayer(world), m_forward(0.f), m_camera{} {
    m_camera.zoom = 50.f;
    m_camera.offset = {raylib::Window::GetWidth() / 2.f, raylib::Window::GetHeight() / 2.f};
}

void LocalPlayer::updateCamera() {
    float wheel = raylib::Mouse::GetWheelMove();

    raylib::Vector2 playerCenter = {
        m_hitbox.x + m_hitbox.width / 2,
        m_hitbox.y - m_hitbox.height / 2
    };

    m_camera.target = playerCenter;
    
    if (raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0) {
        m_camera.zoom -= wheel;

        float scaleX = static_cast<float>(raylib::Window::GetWidth()) / 640;
        float scaleY = static_cast<float>(raylib::Window::GetHeight()) / 360;

        m_camera.zoom = std::clamp(m_camera.zoom, 5.f * scaleX, 90.f * scaleX);
    }

    m_camera.offset = {raylib::Window::GetWidth() / 2.f, raylib::Window::GetHeight() / 2.f};
    
    const float maxX = WORLD_SIZE;   
    const float maxY = WORLD_SIZE;   

    raylib::Vector2 minClamp = m_camera.GetWorldToScreen(raylib::Vector2::Zero());
    raylib::Vector2 maxClamp = m_camera.GetWorldToScreen({maxX, maxY});
    
    auto screenWidth = raylib::Window::GetWidth();
    auto screenHeight = raylib::Window::GetHeight();

    if (maxClamp.x < screenWidth) m_camera.offset.x = screenWidth - (maxClamp.x - screenWidth / 2.f); 
    if (minClamp.x > 0) m_camera.offset.x = screenWidth / 2 - minClamp.x;
    if (maxClamp.y < screenHeight) m_camera.offset.y = screenHeight - (maxClamp.y - screenHeight / 2.f);
}

void LocalPlayer::updateAnimation() {
    float static lastAnimTime = 0.f;

    if (raylib::Window::GetTime() >= lastAnimTime + (1.0f / m_animFps)) {
        lastAnimTime = raylib::Window::GetTime();
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

TilePosition LocalPlayer::getTargetTile() {
    raylib::Vector2 cur = m_camera.GetScreenToWorld(raylib::Mouse::GetPosition());
    auto layer = !raylib::Keyboard::IsKeyDown(KEY_LEFT_ALT);

    return {static_cast<int>(floorl(cur.x)), static_cast<int>(floorl(cur.y)), layer};
}

void LocalPlayer::onTickControls(World& world) {
    auto target = getTargetTile();
    auto mp = Multiplayer::get();
    auto ct = Game::get()->getControlType();

    if (canAccessTile(target)) {
        if (raylib::Mouse::IsButtonDown(MOUSE_LEFT_BUTTON) && canDestroyTile(world, target)) {
            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            destroyTile(world);
        }

        if (m_breakingTile && raylib::Mouse::IsButtonUp(MOUSE_LEFT_BUTTON)) {
            world.stopBreakingTile(m_id);
        }

        if (raylib::Mouse::IsButtonDown(MOUSE_RIGHT_BUTTON) && canPlaceTile(world, target)) {
            if(raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
                destroyTile(world);
            }

            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            placeTile(world);
        }
    }
}

void LocalPlayer::onTick(World& world) {
    Player::onTick(world);

    auto static prevAnimType = m_animType;
    auto static prevAnimFrame = m_animFrame;
    auto static prevDir = m_direction;
    m_animFps = 10;

    onTickControls(world);

    m_speed.x += (m_onGround ? (m_sneak ? 0.0025f : 0.06f) : 0.02f) * m_forward;
    m_speed.y += (!m_fly) ? 0.02f : 0.0f;

    auto hitboxesAround = world.getHitboxes(m_hitbox.expand(m_speed.x, m_speed.y));
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

    if (world.getTime() < m_lastHurtTime + 15) {
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
    auto target = getTargetTile();
    auto tile = world.getTile(target);

    dbg->setString(PLAYER_TARGET_TILE, "Target tile: [{}, {}] ({})", target.x, target.y, (tile != TileID::AIR ? Tile::GetName(tile) : "nullptr"));
    dbg->setString(PLAYER_POSITION, "Position: [{:.2f}, {:.2f}]", m_hitbox.x, m_hitbox.y);
    dbg->setString(PLAYER_HEALTH, "Health: {}", m_health);
}

void LocalPlayer::updateControls() {
    auto stm = SettingsManager::get();
    auto wheel = raylib::Mouse::GetWheelMove();

    if (raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.f) {
        m_camera.zoom -= wheel * 10.f;
    }

    if(raylib::IsGestureDetected(GESTURE_PINCH_IN) || raylib::IsGestureDetected(GESTURE_PINCH_OUT)) {
        m_camera.zoom += raylib::GetGesturePinchAngle();
    }

    if (raylib::Keyboard::IsKeyPressed(stm->getKeybind("fly"))) {
        toggleFly();
    }

    if (raylib::Keyboard::IsKeyDown(stm->getKeybind("right")) || raylib::Keyboard::IsKeyDown(KEY_RIGHT)) {
        triggerMove(RIGHT);
    }

    if (raylib::Keyboard::IsKeyDown(stm->getKeybind("left")) || raylib::Keyboard::IsKeyDown(KEY_LEFT)) {
        triggerMove(LEFT);
    }

    if ((raylib::Keyboard::IsKeyDown(stm->getKeybind("jump")) || raylib::Keyboard::IsKeyDown(KEY_UP))) {
        triggerJump();
    }

    if ((raylib::Keyboard::IsKeyDown(stm->getKeybind("duck"))) || raylib::Keyboard::IsKeyDown(KEY_DOWN)) {
        triggerDuck(false);
    }

    if ((raylib::Keyboard::IsKeyReleased(stm->getKeybind("duck"))) || raylib::Keyboard::IsKeyReleased(KEY_DOWN)) {
        m_sneak = false;
    }

    if (raylib::Keyboard::IsKeyPressed(KEY_F4)) {
        m_gamemode = (m_gamemode == GAMEMODE_CREATIVE) ? GAMEMODE_SURVIVAL : GAMEMODE_CREATIVE;
    }
}

void LocalPlayer::update() {
    updateCamera();
    updateAnimation();
}

bool LocalPlayer::isRectInView(raylib::Rectangle rect) {
    raylib::Vector2 min = m_camera.GetScreenToWorld(raylib::Vector2::Zero());
    raylib::Vector2 max = m_camera.GetScreenToWorld(raylib::Window::GetSize());

    return (rect.x + rect.width >= min.x && rect.x <= max.x) && (rect.y + rect.height >= min.y && rect.y <= max.y);
}

void LocalPlayer::triggerMove(Direction dir) {
    m_direction = dir;
    (dir == RIGHT) ? m_forward++ : m_forward--;

    m_forward = std::clamp(m_forward, -1.f, 1.f);
}

void LocalPlayer::triggerJump() {
    if(m_coyotTime > 0 || m_fly) {
        m_speed.y   = ((!m_fly) ? -0.3f : -0.25f);
        m_coyotTime = 0;
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

void LocalPlayer::placeTile(World& world) {
    if(getItem(m_selected).item != nullptr) {
        auto target = getTargetTile();
        auto mp = Multiplayer::get();

        world.placeTile(m_id, target);

        // if (mp->isConnected()) {
        //     mp->sendPacket(Packet(ObjectHeader::TILE_PLACE, tile->serialize()), TILES);
        // }
    }
}

void LocalPlayer::destroyTile(World& world) {
    auto mp = Multiplayer::get();
    auto target = getTargetTile();

    world.breakTile(m_id, target);

    if (mp->isConnected()) {
        mp->sendPacket(Packet(ObjectHeader::TILE_DESTROY, target), Channel::TILES);
    }
}