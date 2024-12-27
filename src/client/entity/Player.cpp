#include "Player.hpp"
#include <Debug.hpp>
#include <Block.hpp>
#include <World.hpp>
#include <Game.hpp>
#include <Chunk.hpp>
#include <Logger.hpp>
#include <WoolBlock.hpp>

#define WALK_SPEED 10

Player::Player(World* world) : SimplePlayer::SimplePlayer(world) {
    m_header = Header::PLAYER;
    m_texture = LoadTexture("assets/player.png");
    m_camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    m_camera.zoom = 50.0f;
    m_camera.rotation = 0.0f;

    m_inventory.push_back(std::make_shared<Block>(Block::ID::GRASS));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::DIRT));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::STONE));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::COBLESTONE));
    m_inventory.push_back(std::make_shared<Block>(Block::ID::PLANKS));
    for(auto i = 0u; i < 13; i++) {
        m_inventory.push_back(std::make_shared<WoolBlock>(i));
    }

    updateCamera();
}

Player::~Player() {
    UnloadTexture(m_texture);
}

void Player::updateCamera() {
    if (!inputDisabled()) {
        auto wheel = GetMouseWheelMove();

        if (IsKeyDown(KEY_LEFT_CONTROL) && wheel > 0) m_camera.zoom -= 1.0f;
        if (IsKeyDown(KEY_LEFT_CONTROL) && wheel < 0) m_camera.zoom += 1.0f;
    }

    Vector2 new_target = this->m_camera.target;

    if (!m_unlinkCamX) {
        new_target.x = m_hitbox.x + m_hitbox.width / 2;
    }
    if (!m_unlinkCamY) {
        new_target.y = m_hitbox.y - m_hitbox.height / 2;
    }

    this->m_camera.target = new_target;
    m_camera.zoom = std::clamp(m_camera.zoom, 5.f, 90.f);
}

void Player::updateAnimation() {
    if(GetTime() >= m_lastAnimFrameTime + (1.0f / m_animFps)) {
        m_lastAnimFrameTime = GetTime();
        m_animFrame++;

        switch(m_animType) {
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

Vector2 Player::convertToCameraPos(Vector2 pos) {
    return Vector2 {
        (m_camera.target.x - m_camera.offset.x / m_camera.zoom) + pos.x / m_camera.zoom, 
        (m_camera.target.y - m_camera.offset.y / m_camera.zoom) + pos.y / m_camera.zoom
    };
}

Vec2i Player::getTargetBlock(bool onlyExist) {
    auto cur = convertToCameraPos(GetMousePosition());
    auto targetBlock = Vec2i {(int)floorf(cur.x), (int)floorf(cur.y),};

    if (Vector2Distance(cur, {m_hitbox.x, m_hitbox.y}) <= 4 && 
        (!onlyExist || m_world->getBlock(targetBlock.x, targetBlock.y, IsKeyPressed(KEY_LEFT_ALT) ? 0 : 1) != nullptr)) {
        return targetBlock;
    }
    
    return Vec2i {-1, -1};
}

bool Player::canDestroyBlock(Vec2i pos, uint8_t layer) {
    if(m_world->isOutOfBound(pos.x, pos.y, layer)) return false;

    auto block = m_world->getBlock(pos.x, pos.y, layer);
    if(!block) return false;

    Block::ID type = block->getID();
    
    return type != Block::ID::BEDROCK && type != Block::ID::AIR;
}

bool Player::canPlaceBlock(Vec2i pos, uint8_t layer) {
    if(m_world->isOutOfBound(pos.x, pos.y, layer)) return false;

    auto block = m_world->getBlock(pos.x, pos.y, layer);
    auto closedByOtherBlocks = m_world->getBlock(pos.x - 1, pos.y, layer)->getID() == Block::ID::AIR &&
        m_world->getBlock(pos.x + 1, pos.y, layer)->getID() == Block::ID::AIR &&
        m_world->getBlock(pos.x, pos.y - 1, layer)->getID() == Block::ID::AIR &&
        m_world->getBlock(pos.x, pos.y + 1, layer)->getID() == Block::ID::AIR && 
        m_world->getBlock(pos.x, pos.y, !layer)->getID() == Block::ID::AIR;

    if (!block) {
        m_world->setBlock(pos.x, pos.y, layer, std::make_unique<Block>(Block::ID::AIR));
    }

    if (layer == 1 && CheckCollisionRecs(m_hitbox.getRect().to<Rectangle>(), {(float)pos.x, (float)pos.y, 1.0f, 1.0f})) {
        return false;
    }

    if (closedByOtherBlocks) {
        return false;
    }

    return m_world->getBlock(pos.x, pos.y, layer)->getID() == Block::ID::AIR;
}

void Player::updateControls() {
    auto target = getTargetBlock(false);
    auto gravitation = (!m_fly) ? 0.02f : 0.0f;
    auto isAltLayer = IsKeyDown(KEY_LEFT_ALT);
    auto forward = 0.0f;
    auto mp = Game::get()->getMultiplayer();

    m_sneak = false;

    if (!inputDisabled()) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && canDestroyBlock(target, !isAltLayer)) {
            logD("looking into {} {} block", target.x, target.y);
            m_world->destroyBlock(target.x, target.y, !isAltLayer);

            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            if (Game::get()->isMultiplayer()) {
                mp->addToQueue(m_world->getBlock(target.x, target.y, !isAltLayer));
            }
        }

        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && canPlaceBlock(target, !isAltLayer)) {
            auto block = m_inventory[m_selectedBlock];
            m_world->placeBlock(target.x, target.y, !isAltLayer, std::make_shared<Block>(block->getID(), block->getSubID(), target.x, target.y, !isAltLayer));

            if (m_onGround) {
                setAnimation(PLAYER_HIT);
            }

            if (Game::get()->isMultiplayer()) {
                mp->addToQueue(m_world->getBlock(target.x, target.y, !isAltLayer));
            }
        }

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            m_direction = RIGHT;
            forward++;
        }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            m_direction = LEFT;
            forward--;
        }

        if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE)) && (m_onGround || m_fly)) {
            m_speedY = ((!m_fly) ? -0.3f : -0.25f);
        }

        if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) && m_fly && !m_onGround) {
            m_speedY = 0.25f;
        }

        if ((IsKeyDown(KEY_S) || IsKeyDown(KEY_LEFT_SHIFT)) && !m_fly && m_onGround) {
            m_sneak = true;
        }
    }

    moveRelative(forward, (m_onGround ? (m_sneak ? 0.05f : 0.12f) : 0.1f));

    m_speedY += gravitation;

    move(m_speedX, m_speedY);

    m_speedX *= 0.91f;
    m_speedY *= 0.98f;

    if (m_onGround) {
        m_speedX *= 0.7f;
        m_speedY *= 0.7f;
    }

    if(m_fly) {
        m_speedY *= 0.7f;
    }
}

void Player::onTick() {
    Entity::onTick();
    m_prevAnimFrame = m_animFrame;
    m_prevDir = m_direction;
    m_animFps = 10;

    updateControls();

    auto hitLimit = m_animLimits.at(PLAYER_HIT);
    if(m_animFrame < hitLimit.first || m_animFrame >= hitLimit.second) {
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

    auto mp = Game::get()->getMultiplayer();

    if(!Game::get()->isMultiplayer()) {
        auto minX = m_world->convertXtoChunkPosition(m_hitbox.x) - 2;
        auto maxX = m_world->convertXtoChunkPosition(m_hitbox.x) + 2;

        for(auto x = minX; x < maxX; x++) {
            if(!m_world->getChunk(x)) {
                auto chunk = m_world->getGenerator()->generateChunk(x);
                m_world->addChunk(chunk);
            }
        }
    }

    if (Game::get()->isMultiplayer() && (m_prevX != m_hitbox.x || m_prevY != m_hitbox.y || m_prevAnimFrame != m_animFrame || m_prevDir != m_direction)) {
        mp->addToQueue(std::shared_ptr<SimplePlayer>(this));
    }
}

void Player::update() {
	if (!inputDisabled()) {
        auto wheel = GetMouseWheelMove();
        for(int i = 0; i < 6; i++) {
            if(IsKeyDown(KEY_ONE + i)) {
                m_selectedBlock = (Block::ID)(i + 1);
            }
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && wheel > 0) m_camera.zoom -= 1.0f;
        if (IsKeyDown(KEY_LEFT_CONTROL) && wheel < 0) m_camera.zoom += 1.0f;

        if(!IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.0f) {
            m_selectedBlock = (Block::ID)((int)(m_selectedBlock) + (wheel > 0 ? -1 : 1));
            wheel = GetMouseWheelMove();

            if (!IsKeyDown(KEY_LEFT_CONTROL) && wheel != 0.0f) {
                m_selectedBlock += (wheel > 0 ? -1 : 1);

                if (m_selectedBlock > m_inventory.size()) m_selectedBlock = 0;
                if (m_selectedBlock < 0) m_selectedBlock = m_inventory.size();
            }
        }

        if (IsKeyPressed(KEY_R)) resetPosition();
        if (IsKeyPressed(KEY_F)) m_fly = !m_fly;
	}

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

bool Player::isChunkInView(std::shared_ptr<Chunk> chunk) {
    auto minPos = chunk->getPosition() * CHUNK_WIDTH;
    auto maxPos = minPos + CHUNK_WIDTH;
    auto min = convertToCameraPos({0, 0});
    auto max = convertToCameraPos({(float)GetScreenWidth(), (float)GetScreenHeight()});

    return (minPos >= min.x && minPos <= max.x) || (maxPos >= min.x && maxPos <= max.x); 
}

bool Player::isBlockInView(std::shared_ptr<Block> block) {
    if(!block) {
        return false;
    }

    auto pos = block->getPos();
    auto min = convertToCameraPos({0, 0});
    auto max = convertToCameraPos({(float)GetScreenWidth(), (float)GetScreenHeight()});

    return (pos.x + 1.0f >= min.x && pos.x <= max.x) && (pos.y + 1.0f >= min.y && pos.y <= max.y); 
}

bool Player::inputDisabled() {
    return m_inputDisabled;
}
void Player::disableInput(bool flag) {
    m_inputDisabled = flag;
}

void Player::unlinkCameraX(bool flag) {
    m_unlinkCamX = flag;
}
void Player::unlinkCameraY(bool flag) {
    m_unlinkCamY = flag;
}