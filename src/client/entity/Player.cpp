#include <managers/SettingsManager.hpp>
#include <entity/Player.hpp>
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

Player::Player(std::shared_ptr<World> world) : SimplePlayer(world), m_forward(0.f), 
    m_gamemode(GAMEMODE_CREATIVE), m_id(0), m_lastAnimFrameTime(0.f), m_lastDestroyedBlock(0.f), m_lastPlacedBlock(0.f) {
    m_camera.zoom = 50.0f;
    m_camera.rotation = 0.0f;

    // clang-format off
    Col3u const colors[] = {
        {255, 255, 255}, 
        {255, 41, 55}, 
        {255, 128, 0},   
        {255, 255, 0},   
        {0, 255, 0},     
        {0, 255, 255}, 
        {0, 0, 255},
        {128, 0, 255},   
        {255, 0, 255}, 
        {255, 128, 255}, 
        {255, 128, 255}, 
        {128, 128, 128}, 
        {0, 0, 0},     
        {128, 64, 0}
    };
    // clang-format on

    if(m_gamemode == GAMEMODE_CREATIVE) {
        for(int i = 1; i < 20; i++) {
            if(i == ItemID::BEDROCK) {
                continue;
            }

            addItem(std::make_shared<InventoryItem>(static_cast<ItemID>(i), 64));
        }

        for (auto& col : colors) {
            auto wool = std::make_shared<InventoryItem>(WOOL, 64);
            wool->setTag(TagID::TAG_COLOR, col);
            addItem(wool);
        }
    }
}

void Player::updateCamera() {
    auto wheel = GetMouseWheelMove();

    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel > 0) m_camera.zoom -= 1.0f;
    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel < 0) m_camera.zoom += 1.0f;

    m_camera.target.x = m_hitbox.x + m_hitbox.width / 2;
    m_camera.target.y = m_hitbox.y - m_hitbox.height / 2;
    m_camera.offset = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f};
    const int logicalWidth = 1280;
    const int logicalHeight = 720;
    float scaleX = static_cast<float>(GetScreenWidth()) / logicalWidth;
    float scaleY = static_cast<float>(GetScreenHeight()) / logicalHeight;

    m_camera.zoom = std::clamp(m_camera.zoom, 5.f * scaleX, 90.f * scaleX);
}

void Player::updateAnimation() {
    if (GetTime() >= m_lastAnimFrameTime + (1.0f / m_animFps)) {
        m_lastAnimFrameTime = GetTime();
        m_animFrame++;

        switch (m_animType) {
            case PLAYER_IDLE:
                m_animFrame = 0;
                break;
            case PLAYER_MOVE:
                m_animFrame = animationClamp(m_animFrame, 1, 5);
                break;
            case PLAYER_SNEAK:
                m_animFrame = animationClamp(m_animFrame, 6, 7);
                break;
            case PLAYER_JUMP:
                m_animFrame = 8;
                break;
            case PLAYER_HIT:
                m_animFrame = animationClamp(m_animFrame, 9, 13);
                break;
            case PLAYER_HURT:
                m_animFrame = 14;
                break;
            case PLAYER_SIT:
                m_animFrame = 15;
                break;
            case PLAYER_CART:
                m_animFrame = 16;
                break;
        }
    }
}

BlockPosition Player::getTargetBlock() {
    Vector2 cur = TO_CAMERA_POS(m_camera, GetMousePosition());
    auto layer = !IsKeyDown(KEY_LEFT_ALT);

    return {static_cast<int>(floorl(cur.x)), static_cast<int>(floorl(cur.y)), layer};
}

bool Player::canAccessBlock(BlockPosition target) {
    Vec2f playerPos{m_hitbox.x, m_hitbox.y};

    bool result = playerPos.distance(Vec2f {static_cast<float>(target.x), static_cast<float>(target.y)}) <= 4.f;

    return result;
}

bool Player::canDestroyBlock(BlockPosition target) {
    // if (m_world->isOutOfBound(target.x, target.y, target.layer)) {
    //     return false;
    // }

    // if (auto block = m_world->getBlock(target.x, target.y, target.layer)) {
    //     if(block == nullptr) return false;

    //     ItemID type = block->getID();

    //     return type != ItemID::BEDROCK && type > 0;
    // }

    return false;
}

bool Player::canPlaceBlock(BlockPosition target) {
    // if (m_world->isOutOfBound(target.x, target.y, target.layer)) {
    //     return false;
    // }

    // bool blockAround = (
    //     m_world->getBlock(target.x - 1, target.y, target.layer) || 
    //     m_world->getBlock(target.x + 1, target.y, target.layer) ||
    //     m_world->getBlock(target.x, target.y - 1, target.layer) || 
    //     m_world->getBlock(target.x, target.y + 1, target.layer) ||
    //     m_world->getBlock(target.x, target.y, !target.layer)
    // );

    // bool overlap = false;

    // for(auto& [_, player] : m_world->getPlayers()) {
    //     overlap |= (target.layer == 1 && CheckCollisionRecs(player->getHitbox().getRect().to<Rectangle>(), BLOCK_RECT(target.x, target.y)));
    // }

    // return !m_world->getBlock(target.x, target.y, target.layer) && blockAround && !overlap;
    return true;
}

void Player::onTickControls() {
    auto target = getTargetBlock();
    auto mp = Multiplayer::get();
    auto ct = Game::get()->getControlType();

    if (canAccessBlock(target)) {
        bool wantPlaceBlock = false;
        bool wantDestroyBlock = false;

        if(ct == CONTROL_KEYBOARD_MOUSE) {
            wantPlaceBlock = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
            wantDestroyBlock = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        }

        if(ct == CONTROL_TOUCH) {
            wantDestroyBlock = IsGestureDetected(GESTURE_HOLD) && GetGestureHoldDuration() > 0.5f;
            wantPlaceBlock = !wantDestroyBlock && IsGestureDetected(GESTURE_TAP);
        }

        // m_isBreakingBlock = wantDestroyBlock && m_world->getBlock(target.x, target.y, target.layer);

        if(m_isBreakingBlockPrev && !m_isBreakingBlock) {
            m_breakingBlock = {0, -1};
        }

        m_isBreakingBlockPrev = m_isBreakingBlock;

        if (wantDestroyBlock && GetTime() >= m_lastDestroyedBlock + 0.1f && canDestroyBlock(target)) {
            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            destroyBlock();
        }

        if (wantPlaceBlock && GetTime() >= m_lastPlacedBlock + 0.05f && canPlaceBlock(target)) {
            if(IsKeyDown(KEY_LEFT_CONTROL)) {
                destroyBlock();
            }

            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            placeBlock();
        }
    }
}

void Player::onTick(World* world) {
    SimplePlayer::onTick(world);

    m_prevAnimType = m_animType;
    m_prevAnimFrame = m_animFrame;
    m_prevDir = m_direction;
    m_animFps = 10;

    onTickControls();

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

    auto hitLimit = getAnimLimit(m_animType);
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
        m_prevAnimFrame != m_animFrame || 
        m_prevAnimType != m_animType || 
        m_prevDir != m_direction
    );
    // clang-format on

    if (mp->isConnected() && shouldupd) {
        mp->sendObj(std::shared_ptr<SimplePlayer>(Game::get()->getPlayer()), EVERYTHING, false);
    }

    auto dbg = Debug::get();
    auto target = getTargetBlock();
    auto block = world->getBlock(target);

    dbg->setString(PLAYER_TARGET_BLOCK, "Target block: [{}, {}] ({})", target.x, target.y, (block ? block->getName() : "nullptr"));
    dbg->setString(PLAYER_POSITION, "Position: [{:.2f}, {:.2f}]", m_hitbox.x, m_hitbox.y);
    dbg->setString(PLAYER_HEALTH, "Health: {}", m_health);
}

void Player::updateControls() {
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
}

void Player::update() {
    updateCamera();
    updateAnimation();
}

bool Player::isChunkInView(Vec2i chunkPos) {
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

bool Player::isBlockInView(BlockPosition position) {
    Vec2i zero = {0, 0};
    Vec2i screen = {GetScreenWidth(), GetScreenHeight()};

    Vector2 min = TO_CAMERA_POS(m_camera, zero);
    Vector2 max = TO_CAMERA_POS(m_camera, screen);

    return (position.x + 1.0f >= min.x && position.x <= max.x) && (position.y + 1.0f >= min.y && position.y <= max.y);
}

void Player::triggerMove(Direction dir) {
    m_direction = dir;
    (dir == RIGHT) ? m_forward++ : m_forward--;

    m_forward = std::clamp(m_forward, -1.f, 1.f);
}

void Player::triggerJump() {
    if(m_onGround || m_fly) {
        m_speed.y = ((!m_fly) ? -0.3f : -0.25f);
    }
}

void Player::toggleFly() {
    if(m_gamemode == GAMEMODE_CREATIVE) {
        m_fly = !m_fly;
    }
}

void Player::triggerDuck(bool toggle) {
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

void Player::setGameMode(GameMode gamemode) {
    if(gamemode == GAMEMODE_SURVIVAL) {
        m_fly = false;
    }

    m_gamemode = gamemode;
}

void Player::placeBlock() {
    // if(getItem(m_selected) != nullptr) {
    //     auto block = Block::create(reinterpret_cast<Item&>(*m_inventory[m_selected]));
    //     auto target = getTargetBlock();
    //     auto mp = Multiplayer::get();

    //     m_world->placeBlock(target.x, target.y, target.layer, block);
        
    //     if(m_gamemode == GAMEMODE_SURVIVAL) {
    //         m_inventory[m_selected]->sub(1);

    //         if(m_inventory[m_selected]->getCount() <= 0) {
    //             m_inventory[m_selected] = nullptr;
    //         }
    //     }

    //     m_lastPlacedBlock = GetTime();

    //     if (mp->connected()) {
    //         mp->sendPacket(Packet(ObjectHeader::BLOCK_PLACE, block->serialize()), BLOCKS);
    //     }
    // }
}

void Player::destroyBlock() {
    auto mp = Multiplayer::get();
    auto target = getTargetBlock();
#if 0
    auto targetBlock = m_world->getBlock(target.x, target.y, target.layer);

    if (m_breakingBlock != target) {
        m_breakingBlock = target;
        m_breakingBlockDurability = 0.f;
        m_lastPunch = GetTime();
    }

    if (m_gamemode == GAMEMODE_SURVIVAL && targetBlock != nullptr) {
        if (GetTime() > m_lastPunch + 0.1f) {
            int strength = 1;
            auto curItem = m_inventory[m_selected];

            bool correctItem = (
                curItem != nullptr &&
                gToolForMaterial.find(curItem->getType()) != gToolForMaterial.end() &&
                gToolForMaterial[curItem->getType()] == targetBlock->getMaterial()
            );

            if (correctItem && curItem->hasTag(TAG_EFFICIENCY)) {
                strength += curItem->getTag<uint8_t>(TAG_EFFICIENCY);
            }

            m_breakingBlockDurability -= strength;
            m_lastPunch = GetTime();
        }

        if (m_breakingBlockDurability > 0) {
            return;
        }

        addItem(targetBlock->dropItem(m_inventory[m_selected]));
    }

    m_world->destroyBlock(target.x, target.y, target.layer);
    m_lastDestroyedBlock = GetTime();
#endif

    if (mp->isConnected()) {
        auto pak = Packet(ObjectHeader::BLOCK_DESTROY);
        pak.add<int32_t>(target.x);
        pak.add<int32_t>(target.y);
        pak.add<uint8_t>(target.layer);
        
        mp->sendPacket(pak, BLOCKS);
    }
}

BreakingBlockInfo Player::getBreakingBlockInfo() {
    // auto target = getTargetBlock();
    // auto targetBlock = m_world->getBlock(target.x, target.y, target.layer);

    // if (!targetBlock || !m_isBreakingBlock) {
    //     return {0, 1};
    // }

    return {/* targetBlock->getDurability() */ 0.f, m_breakingBlockDurability};
}