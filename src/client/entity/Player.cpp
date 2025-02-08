#include <entity/Player.hpp>
#include <world/Block.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <Multiplayer.hpp>
#include <Debug.hpp>
#include <Utils.hpp>
#include <Game.hpp>
#include <SettingsManager.hpp>
Player::Player(std::shared_ptr<World> world) : SimplePlayer::SimplePlayer(world) {
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

    m_inventory.push_back(std::make_shared<Block>(Block::ID::GRASS));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::DIRT));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::STONE));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::COBLESTONE));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::PLANKS));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::BRICKS));

    for (auto& col : colors) {
        auto wool = std::make_shared<Block>(Block::ID::WOOL);
        wool->setTag(Block::TagID::COLOR, col);
        m_inventory.push_back(wool);
    }

    updateCamera();
}

void Player::updateCamera() {
    auto wheel = GetMouseWheelMove();

    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel > 0)
        m_camera.zoom -= 1.0f;
    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel < 0)
        m_camera.zoom += 1.0f;

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

Vec2i Player::getTargetBlock() {
    Vector2 cur = TO_CAMERA_POS(m_camera, GetMousePosition());

    return {static_cast<int>(floorl(cur.x)), static_cast<int>(floorl(cur.y))};
}

bool Player::canAccessBlock(Vec2i target, uint8_t layer) {
    bool result = target.distance(Vec2f {m_hitbox.x, m_hitbox.y}) <= 4;

    return result;
}

bool Player::canDestroyBlock(Vec2i pos, uint8_t layer) {
    if (m_world->isOutOfBound(pos.x, pos.y, layer)) {
        return false;
    }

    if (auto block = m_world->getBlock(pos.x, pos.y, layer)) {
        if(block == nullptr) return false;

        Block::ID type = block->getID();

        return type != Block::ID::BEDROCK && type > 0;
    }

    return false;
}

bool Player::canPlaceBlock(Vec2i pos, uint8_t layer) {
    if (m_world->isOutOfBound(pos.x, pos.y, layer)) {
        return false;
    }

    bool blockAround = (
        m_world->getBlock(pos.x - 1, pos.y, layer) || 
        m_world->getBlock(pos.x + 1, pos.y, layer) ||
        m_world->getBlock(pos.x, pos.y - 1, layer) || 
        m_world->getBlock(pos.x, pos.y + 1, layer) ||
        m_world->getBlock(pos.x, pos.y, !layer)
    );

    bool overlap = false;

    for(auto& [_, player] : m_world->getPlayers()) {
        overlap |= (layer == 1 && CheckCollisionRecs(player->getHitbox().getRect().to<Rectangle>(), BLOCK_RECT(pos.x, pos.y)));
    }

    return !m_world->getBlock(pos.x, pos.y, layer) && blockAround && !overlap;
}

void Player::updateControls() {
    auto target = getTargetBlock();
    auto gravitation = (!m_fly) ? 0.02f : 0.0f;
    auto layer = !IsKeyDown(KEY_LEFT_ALT);
    auto forward = 0.0f;
    auto mp = Multiplayer::get();
    auto dbg = Debug::get();

    m_sneak = false;

    if (canAccessBlock(target, layer)) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && canDestroyBlock(target, layer)) {
            m_world->destroyBlock(target.x, target.y, layer);
            
            if(IsKeyDown(KEY_LEFT_CONTROL)) {
                auto block = std::make_shared<Block>(*m_inventory[m_selectedBlock]);
                m_world->placeBlock(target.x, target.y, layer, block);
            }

            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            if (mp->connected()) {
                // mp->onBlockChanged({target.x, target.y}, layer);
                auto pak = Packet(Header::BLOCK_DESTROY);
                pak.add<int32_t>(target.x);
                pak.add<int32_t>(target.y);
                pak.add<uint8_t>(layer);
                
                mp->sendPacket(pak, BLOCKS);
            }
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && canPlaceBlock(target, layer)) {
            auto block = std::make_shared<Block>(*m_inventory[m_selectedBlock]);
            m_world->placeBlock(target.x, target.y, layer, block);

            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            if (mp->connected()) {
                mp->sendPacket(Packet(Header::BLOCK_PLACE, block->serialize()), BLOCKS);
            }
        }
    }
    auto stm = SettingsManager::get();
    if (IsKeyDown(stm->getKeybind("Right"))) {
        m_direction = RIGHT;
        forward++;
    }

    if (IsKeyDown(stm->getKeybind("Left")) || IsKeyDown(KEY_LEFT)) {
        m_direction = LEFT;
        forward--;
    }

    if (IsKeyDown(stm->getKeybind("Jump"))  && (m_onGround || m_fly)) {
        m_speedY = ((!m_fly) ? -0.3f : -0.25f);
    }

    if (IsKeyDown(stm->getKeybind("Duck")) && m_fly && !m_onGround) {
        m_speedY = 0.25f;
    }

    if (IsKeyDown(stm->getKeybind("Duck")) && !m_fly && m_onGround) {
        m_sneak = true;
    }

    m_speedX += (m_onGround ? (m_sneak ? 0.0025f : 0.06f) : 0.02f) * forward;
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

    auto block = m_world->getBlock(target.x, target.y, layer);

    dbg->setString(PLAYER_TARGET_BLOCK, "Target block: [{}, {}] ({})", target.x, target.y, (block ? Block::idToString(block->getID()) : "nullptr"));
    dbg->setString(PLAYER_POSITION, "Position: [{:.2f}, {:.2f}]", m_hitbox.x, m_hitbox.y);
}

void Player::onTick() {
    Entity::onTick();
    m_prevAnimType = m_animType;
    m_prevAnimFrame = m_animFrame;
    m_prevDir = m_direction;
    m_animFps = 10;

    updateControls();

    auto hitLimit = m_animLimits.at(PLAYER_HIT);
    if (m_animFrame < hitLimit.first || m_animFrame >= hitLimit.second) {
        setAnimation(PLAYER_IDLE);
    }

    if (m_sneak) {
        setAnimation(PLAYER_SNEAK);
        m_animFps = 0;
    }

    if (m_onGround && (m_speedX > 0.025f || m_speedX < -0.025f)) {
        m_animFps = (m_sneak ? 7 : 10);
        setAnimation((m_sneak ? PLAYER_SNEAK : PLAYER_MOVE));
    }

    if (!m_onGround) {
        setAnimation(PLAYER_JUMP);
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

void Player::update() {
    for (int i = 0; i < 9; i++) {
        if (IsKeyDown(KEY_ONE + i)) {
            m_selectedBlock = (Block::ID)(i + 1);
        }
    }

    auto wheel = GetMouseWheelMove();
    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel > 0)
        m_camera.zoom -= 1.0f;
    if (IsKeyDown(KEY_LEFT_CONTROL) && wheel < 0)
        m_camera.zoom += 1.0f;

    if (!IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.0f) {
        m_selectedBlock += (wheel > 0 ? -1 : 1);
        if (m_selectedBlock >= m_inventory.size())
            m_selectedBlock = 0;
        if (m_selectedBlock < 0)
            m_selectedBlock = static_cast<uint8_t>(m_inventory.size()) - 1;
    }
    auto stm = SettingsManager::get();

    if (IsKeyPressed(KEY_R))
        resetPosition();

    if (IsKeyPressed(stm->getKeybind("Fly")))
        m_fly = !m_fly;
    if(IsKeyPressed(KEY_E)) 
        m_inv = !m_inv;
    updateCamera();
    updateAnimation();
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

std::shared_ptr<Block> Player::getSelectedBlock() {
    if (m_selectedBlock < m_inventory.size()) {
        return m_inventory[m_selectedBlock];
    }

    return nullptr;
}