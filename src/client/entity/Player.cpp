#include <SettingsManager.hpp>
#include <entity/Player.hpp>
#include <world/Block.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <Multiplayer.hpp>
#include <Debug.hpp>
#include <Utils.hpp>
#include <Game.hpp>
#include <algorithm>
#include <Types.hpp>

#undef min

Player::Player(std::shared_ptr<World> world) 
    : SimplePlayer(world), m_selectedBlock(0), m_inventory(36), m_forward(0.f), m_gamemode(GAMEMODE_SURVIVAL), 
    m_id(0), m_lastAnimFrameTime(0.f), m_lastDestroyedBlock(0.f), m_lastPlacedBlock(0.f) {
    m_header = Header::PLAYER;
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
            if(i == BlockID::BEDROCK) {
                continue;
            }

            addToInventory({std::make_shared<ItemBase>(i), INVENTORY_TYPE_BLOCK, 64});
        }

        for (auto& col : colors) {
            auto wool = std::make_shared<Block>(BlockID::WOOL);
            wool->setTag(TagID::TAG_COLOR, col);
            addToInventory({wool, INVENTORY_TYPE_BLOCK, 64});
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
    if (m_world->isOutOfBound(target.x, target.y, target.layer)) {
        return false;
    }

    if (auto block = m_world->getBlock(target.x, target.y, target.layer)) {
        if(block == nullptr) return false;

        BlockID type = block->getID();

        return type != BlockID::BEDROCK && type > 0;
    }

    return false;
}

bool Player::canPlaceBlock(BlockPosition target) {
    if (m_world->isOutOfBound(target.x, target.y, target.layer)) {
        return false;
    }

    bool blockAround = (
        m_world->getBlock(target.x - 1, target.y, target.layer) || 
        m_world->getBlock(target.x + 1, target.y, target.layer) ||
        m_world->getBlock(target.x, target.y - 1, target.layer) || 
        m_world->getBlock(target.x, target.y + 1, target.layer) ||
        m_world->getBlock(target.x, target.y, !target.layer)
    );

    bool overlap = false;

    for(auto& [_, player] : m_world->getPlayers()) {
        overlap |= (target.layer == 1 && CheckCollisionRecs(player->getHitbox().getRect().to<Rectangle>(), BLOCK_RECT(target.x, target.y)));
    }

    return !m_world->getBlock(target.x, target.y, target.layer) && blockAround && !overlap;
}

void Player::onTickControls() {
    auto target = getTargetBlock();
    auto gravitation = (!m_fly) ? 0.02f : 0.0f;
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

        m_isBreakingBlock = wantDestroyBlock && m_world->getBlock(target.x, target.y, target.layer);

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

    m_speedX += (m_onGround ? (m_sneak ? 0.0025f : 0.06f) : 0.02f) * m_forward;
    m_speedY += gravitation;

    move(m_speedX, m_speedY);

    m_speedX *= 0.91f;
    m_speedY *= 0.98f;

    if (m_onGround) {
        m_speedX *= 0.7f;
        m_speedY *= 0.7f;
    }

    if (m_fly) {
        m_speedY *= 0.7f;
    }

    m_forward = 0.f;
}

void Player::onTick() {
    SimplePlayer::onTick();

    m_prevAnimType = m_animType;
    m_prevAnimFrame = m_animFrame;
    m_prevDir = m_direction;
    m_animFps = 10;

    onTickControls();

    auto hitLimit = m_animLimits.at(PLAYER_HIT);
    if (m_animFrame < hitLimit.first || m_animFrame >= hitLimit.second) {
        setAnimation(PLAYER_IDLE);
    }

    if (m_onGround && (m_speedX > 0.025f || m_speedX < -0.025f)) {
        m_animFps = (m_sneak ? 7 : 10);
        setAnimation((m_sneak ? PLAYER_SNEAK : PLAYER_MOVE));
    }

    if (m_sneak) {
        setAnimation(PLAYER_SNEAK);
        m_animFps = (m_speedX != 0.f ? 4 : 0);
    }

    if (!m_onGround) {
        setAnimation(PLAYER_JUMP);
    }

    if (GetTime() < m_lastHurtTime + 0.25f) {
        setAnimation(PLAYER_HURT);
    }

    auto mp = Multiplayer::get();

    auto minX = m_world->xToChunk(m_hitbox.x) - 2;
    auto maxX = m_world->xToChunk(m_hitbox.x) + 2;

    for(auto x = minX; x < maxX; x++) {
        if(m_world->getChunk(x)) continue;

        if(mp->connected()) {
            mp->requestChunk(x);
        } else {
            auto chunk = m_world->getGenerator()->generateChunk(x);
            m_world->addChunk(chunk);
        }
    }

    if(!m_sneakToggled) {
        m_sneak = false;
    }

    // clang-format off
    bool shouldupd = (
        m_prevX != m_hitbox.x || 
        m_prevY != m_hitbox.y || 
        m_prevAnimFrame != m_animFrame || 
        m_prevAnimType != m_animType || 
        m_prevDir != m_direction
    );
    // clang-format on

    if (mp->connected() && shouldupd) {
        mp->sendObj(std::shared_ptr<SimplePlayer>(Game::get()->getPlayer()), EVERYTHING, false);
    }
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

    auto dbg = Debug::get();
    auto target = getTargetBlock();
    auto block = m_world->getBlock(target.x, target.y, target.layer);

    dbg->setString(PLAYER_TARGET_BLOCK, "Target block: [{}, {}] ({})", target.x, target.y, (block ? Block::idToString(block->getID()) : "nullptr"));
    dbg->setString(PLAYER_POSITION, "Position: [{:.2f}, {:.2f}]", m_hitbox.x, m_hitbox.y);
    dbg->setString(PLAYER_HEALTH, "Health: {}", m_health);
}

bool Player::isChunkInView(std::shared_ptr<Chunk> chunk) {
    auto minPos = chunk->getPosition() * CHUNK_WIDTH;
    auto maxPos = minPos + CHUNK_WIDTH;
    Vec2i zero = {0, 0};
    Vec2i screen = {GetScreenWidth(), GetScreenHeight()};
    Vector2 min = TO_CAMERA_POS(m_camera, zero);
    Vector2 max = TO_CAMERA_POS(m_camera, screen);

    return (minPos >= min.x && minPos <= max.x) || (maxPos >= min.x && maxPos <= max.x) || (min.x >= minPos && max.x <= maxPos);
}

bool Player::isBlockInView(std::shared_ptr<Block> block) {
    if (block != nullptr) {
        Vec2i zero = {0, 0};
        Vec2i screen = {GetScreenWidth(), GetScreenHeight()};

        auto pos = block->getPos();
        Vector2 min = TO_CAMERA_POS(m_camera, zero);
        Vector2 max = TO_CAMERA_POS(m_camera, screen);

        return (pos.x + 1.0f >= min.x && pos.x <= max.x) && (pos.y + 1.0f >= min.y && pos.y <= max.y);
    }

    return false;
}

InventoryItem const& Player::getSelectedItem() {
    return m_inventory[m_selectedBlock % 9];
}

int Player::addToInventory(InventoryItem item) {
    if(item.count <= 0) return 0;

    auto isBlock = std::dynamic_pointer_cast<Block>(item.pointer) != nullptr;

    for (auto& slot : m_inventory) {
        if (slot.pointer == nullptr || *slot.pointer == item.pointer) {  
            uint16_t add = std::min(item.count, (uint16_t)(slot.pointer != nullptr ? 64 - slot.count : 64));

            if (slot.pointer == nullptr) {
                slot = InventoryItem {item.pointer, item.type, 0};  
            }

            slot.count += add;
            item.count -= add;

            if (item.count <= 0) return 0;
        }
    }

    return item.count;
}

void Player::triggerMove(Direction dir) {
    m_direction = dir;
    (dir == RIGHT) ? m_forward++ : m_forward--;

    m_forward = std::clamp(m_forward, -1.f, 1.f);
}

void Player::triggerJump() {
    if(m_onGround || m_fly) {
        m_speedY = ((!m_fly) ? -0.3f : -0.25f);
    }
}

void Player::toggleFly() {
    if(m_gamemode == GAMEMODE_CREATIVE) {
        m_fly = !m_fly;
    }
}

void Player::triggerDuck(bool toggle) {
    if (m_fly && !m_onGround) {
        m_speedY = 0.25f;
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
    if(m_inventory[m_selectedBlock].pointer != nullptr) {
        auto block = std::make_shared<Block>(*m_inventory[m_selectedBlock].pointer);
        auto target = getTargetBlock();
        auto mp = Multiplayer::get();

        m_world->placeBlock(target.x, target.y, target.layer, block);
        
        if(m_gamemode == GAMEMODE_SURVIVAL) {
            m_inventory[m_selectedBlock].count--;

            if(m_inventory[m_selectedBlock].count <= 0) {
                m_inventory[m_selectedBlock].pointer = nullptr;
            }
        }

        m_lastPlacedBlock = GetTime();

        if (mp->connected()) {
            mp->sendPacket(Packet(Header::BLOCK_PLACE, block->serialize()), BLOCKS);
        }
    }
}

void Player::destroyBlock() {
    auto mp = Multiplayer::get();
    auto target = getTargetBlock();
    auto targetBlock = m_world->getBlock(target.x, target.y, target.layer);

    if (m_breakingBlock != target) {
        m_breakingBlock = target;
        m_breakingBlockDurability = (targetBlock != nullptr ? targetBlock->getDurability() : 0.f);
        m_lastPunch = GetTime();
    }

    if (m_gamemode == GAMEMODE_SURVIVAL && targetBlock != nullptr) {
        if (GetTime() > m_lastPunch + 0.1f) {
            m_breakingBlockDurability--;
            m_lastPunch = GetTime();
        }

        if (m_breakingBlockDurability > 0) {
            return;
        }

        addToInventory(targetBlock->dropItem());
    }
    
    m_world->destroyBlock(target.x, target.y, target.layer);
    m_lastDestroyedBlock = GetTime();

    if (mp->connected()) {
        auto pak = Packet(Header::BLOCK_DESTROY);
        pak.add<int32_t>(target.x);
        pak.add<int32_t>(target.y);
        pak.add<uint8_t>(target.layer);
        
        mp->sendPacket(pak, BLOCKS);
    }
}

BreakingBlockInfo Player::getBreakingBlockInfo() {
    auto target = getTargetBlock();
    auto targetBlock = m_world->getBlock(target.x, target.y, target.layer);

    if (!targetBlock || !m_isBreakingBlock) {
        return {0, 1};
    }

    return {targetBlock->getDurability(), m_breakingBlockDurability};
}