#include <player.hpp>
#include <Debug.hpp>
#include "Block.hpp"
#include <GenericTools.hpp>
#include <world.hpp>
#define WALK_SPEED 200
#define JUMP_SPEED 350
#define G 400
#define BS BLOCK_SIZE_PIXELS
#define WALL_PADDING 2
#define WP WALL_PADDING

Player::Player(World* world) {
    m_world = world;
    m_objectID = 1;
    this->m_texture = LoadTexture("assets/player.png");
    this->m_rect = {(float)(rand() % 256 * BLOCK_SIZE_PIXELS), 1250, 20.f, 41.f};
    this->updateCamera();
}

void Player::updateCamera() {
    this->camera.target = {m_rect.x + m_rect.width / 2, m_rect.y - m_rect.height / 2};
    this->camera.zoom = 1.5f;
    this->camera.rotation = 0.0f;

    Debug::addString(TextFormat("Camera target: [%.0f, %.0f]", this->camera.target.x, this->camera.target.y));
    Debug::addString(TextFormat("Camera zoom: %.02f", this->camera.zoom));
    Debug::addString(TextFormat("Camera rotation: %.02f", this->camera.rotation));
}
void Player::player_breakBlock(std::vector<Rectangle> envHitboxes) {
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for(auto& hitbox : envHitboxes) {
            if(CheckCollisionPointRec(cursorInWorld,hitbox)) {
                m_world->destroyBlock((int){hitbox.x} / BLOCK_SIZE_PIXELS,(int){hitbox.y} / BLOCK_SIZE_PIXELS);
            }
        }
    } 
}
void Player::player_placeBlock(std::vector<Rectangle> envHitboxes) {
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !CheckCollisionPointRec(cursorInWorld,m_rect)) {
            m_world->placeBlock((int){cursorInWorld.x} / BLOCK_SIZE_PIXELS,(int){cursorInWorld.y} / BLOCK_SIZE_PIXELS,Block::BlockType::DIRT);
    }
}
void Player::update(std::vector<Rectangle> envHitboxes) {
    cursor = GetMousePosition();
    cursorInWorld = {(camera.target.x - camera.offset.x / camera.zoom) + cursor.x / camera.zoom, (camera.target.y - camera.offset.y / camera.zoom) + cursor.y / camera.zoom};
    player_breakBlock(m_world->m_hitboxes);
    player_placeBlock(m_world->m_hitboxes);


    for(auto& hitbox : envHitboxes) {
    Debug::addString(TextFormat("Block hitbox / 32: %.i, %i",(int){hitbox.x} / BLOCK_SIZE_PIXELS,(int){hitbox.y} / BLOCK_SIZE_PIXELS));
    }

    auto delta = GetFrameTime();

    // m_speed.x = 0.0f;

    // if (IsKeyDown(KEY_D)) {
    //     m_direction = 1;
    //     m_speed.x = WALK_SPEED;
    // }

    // if (IsKeyDown(KEY_A)) {
    //     m_direction = -1;
    //     m_speed.x = WALK_SPEED;
    // }

    // if (IsKeyDown(KEY_W) && (m_canJump || m_fly)) {
    //     if(m_fly) {
    //         m_rect.y -= WALK_SPEED * delta;
    //     } else {
    //         m_speed.y = -JUMP_SPEED;
    //         m_canJump = false;
    //     }
    // }

    // if(IsKeyDown(KEY_S) && m_fly) {
    //     m_rect.y += WALK_SPEED * delta;
    // }

    // if (IsKeyDown(KEY_R)) {
    //     m_rect.x = (float)(rand() % 256 * BLOCK_SIZE_PIXELS);
    //     m_rect.y = 0.0f;
    // }

    // if (IsKeyPressed(KEY_F)) {
    //     m_fly = !m_fly;
    // }

    _objects = envHitboxes;

    processMovement();

    processXAcceleration();

    fixPlayerX();

    processColliding();

    processYAcceleration();
    processGravity();

    bool hitFloor = false;
    bool hitWall = false;

    for(auto& bh : envHitboxes) {
        // // hit right wall
        // if(bh.y >= m_rect.y && bh.y + bh.height <= m_rect.y + m_rect.height && m_rect.x + m_rect.width >= bh.x && m_rect.x + m_rect.width <= bh.x + bh.width) {
        //     m_rect.x = bh.x - m_rect.width;          
        //     m_speed.x = 0.0f;
        //     hitWall = true;
        // }

        // // hit left wall
        // if(bh.y >= m_rect.y && bh.y + bh.height <= m_rect.y + m_rect.height && bh.x + bh.width <= m_rect.x) {
        //     m_rect.x = bh.x + bh.width;
        //     m_speed.x = 0.0f;
        //     hitWall = true;
        // }

        // // hit the ceil
        // if(CheckCollisionPointRec({m_rect.x, m_rect.y}, {bh.x * BS, bh.y * BS + (BS / 2), BS, BS / 2}) ||
        //    CheckCollisionPointRec({m_rect.x + m_rect.width, m_rect.y}, {bh.x * BS, bh.y * BS + (BS / 2), BS, BS / 2})) {
        //     m_rect.y = ((bh.y + 1) * BS);
        //     m_speed.y = 0.0f;
        // }

        // // hit the floor
        // if(CheckCollisionPointRec({m_rect.x, m_rect.y + m_rect.height}, {bh.x * BS, bh.y * BS, BS, BS / 2}) ||
        //    CheckCollisionPointRec({m_rect.x + m_rect.width, m_rect.y + m_rect.height}, {bh.x * BS, bh.y * BS, BS, BS / 2})) {
        //     m_rect.y = bh.y * BS - m_rect.height;
        //     m_speed.y = 0.0f;
        //     hitFloor = true;
        // }
    }

    // if(!hitFloor && !m_fly) {
    //     m_rect.y += m_speed.y * delta;
    //     m_speed.y += G * delta;
    //     m_canJump = false;
    // } else {
    //     m_canJump = true;
    // }

    // if(!hitWall) {
    //     m_rect.x += m_speed.x * m_direction * delta;
    // }

    this->updateCamera();

    Debug::addString(TextFormat("Player position: [%.0f, %.0f]", this->m_rect.x, this->m_rect.y));
    Debug::addString(TextFormat("Hit floor: %d", hitFloor));
    Debug::addString(TextFormat("Hit wall: %d", hitWall));
    Debug::addString(TextFormat("Fly: %d", m_fly));
}

void Player::draw() {
   DrawRectangle((int){cursorInWorld.x}, (int){cursorInWorld.y},10,10,MAROON);
    DrawTexturePro( 
        this->m_texture, 
        {0, 0, (float)m_texture.width * m_direction, (float)m_texture.height}, 
        {m_rect.x, m_rect.y, (float)m_texture.width, (float)m_texture.height},
        {-m_rect.width / 2 + m_texture.width / 2, m_texture.height - m_rect.height}, 0, WHITE
    );

    if(Debug::m_debug) {
        DrawRectangleLinesEx(m_rect, 1.0f, GREEN);

        auto parts = splitPlayerHitbox4();

        std::unordered_map<std::string, Color> colors = {
            {"top-right-corner", {255, 0, 0, 64}},
            {"top-left-corner", {128, 255, 0, 64}},
            {"bottom-right-corner", {0, 255, 128, 64}},
            {"bottom-left-corner", {0, 0, 255, 64}},
            {"left-side", {255, 0, 0, 255}},
            {"right-side", {0, 0, 255, 255}},
            {"top", {255, 0, 255, 255}},
            {"bottom", {255, 128, 0, 255}}
        };

        // for (auto [k, v] : parts) {
        //     DrawRectangleRec(v, colors[k]);
        // }

        parts = splitPlayerHitbox2V();

        for (auto [k, v] : parts) {
            DrawRectangleLinesEx(v, 1.5f, colors[k]);
        }

        // parts = splitPlayerHitbox2H();

        // for (auto [k, v] : parts) {
        //     DrawRectangleLinesEx(v, 1.f, colors[k]);
        // }
    }
}

Player::SObject Player::encodeObject() {
    SObject obj = SerializedObject::encodeObject();

    GenericTools::addVectors(
        &obj,
        GenericTools::valueToVector<float>(&m_rect.x)
    );
    GenericTools::addVectors(
        &obj,
        GenericTools::valueToVector<float>(&m_rect.y)
    );

    return obj;
};
int Player::decodeObject(SObject &s) {
    unsigned int _offset = SerializedObject::decodeObject(s);
    unsigned int offset = _offset;
    unsigned int required = sizeof(float) * 2;
    
    if (required > s.size() - offset) return s.size() - offset;

    m_rect.x = GenericTools::vectorToValue<float>(s, offset);
    offset += sizeof(float);

    m_rect.y = GenericTools::vectorToValue<float>(s, offset);
    offset += sizeof(float);

    return s.size();
}

float Player::getMaxSpeed() {
    return 1.5f * _delta * 140;
}
float Player::getStopSpeed() {
    return 4.5f * _delta * 130.f;
}
float Player::getAccelerationValue() {
    return 8.f * _delta * 56;
}

void Player::moveRight() {
    _lookingToRight = true;
    _finishRight = false;

    // try predict walls

    int val = 2;

    m_rect.x++;

    processColliding();
    
    if (inWall()) {
        m_rect.x--;
        // _accelX = 0;
        return;
    }

    m_rect.x--;
    processColliding();

    if (_accelX < getMaxSpeed()) {
        this->_accelX += _delta * getAccelerationValue();
    } else {
        _accelX = getMaxSpeed();
    }
}

void Player::moveLeft() {
    _lookingToRight = false;
    _finishLeft = false;

    m_rect.x--;

    processColliding();
    
    if (inWall()) {
        m_rect.x++;
        // _accelX = 0;
        return;
    }

    m_rect.x++;
    processColliding();

    if (_accelX > -getMaxSpeed()) {
        this->_accelX -= _delta * getAccelerationValue();
    } else {
        _accelX = -getMaxSpeed();
    }
}

void Player::releaseMovementLeft() {
    _finishLeft = true;
}

void Player::releaseMovementRight() {
    _finishRight = true;
}

void Player::processMovement() { 
    if (IsKeyReleased(KEY_D)) releaseMovementRight();
    if (IsKeyReleased(KEY_A)) releaseMovementLeft();

    if (IsKeyPressed(KEY_D)) {
        m_direction = 1;
    } else if (IsKeyPressed(KEY_A)) {
        m_direction = -1;
    }

    if (IsKeyPressed(KEY_SPACE)) {
        jump(false);
    }
    // if (IsKeyDown(KEY_SPACE)) {
    //     jump(IsKeyDown(KEY_SPACE));
    // }
    
    bool moving_right = IsKeyDown(KEY_D);
    bool moving_left = IsKeyDown(KEY_A);

    if (moving_right) moveRight();
    if (moving_left) moveLeft();
}

void Player::processGravity() {
    //printf("standing floor: %f %f %f %f\n", _standingObject.x, _standingObject.y, _standingObject.width, _standingObject.height);

    float max_accel = 1.5f;

    if (isFalling() && !_jumpRequested) {
        if (_accelY > -getMaxSpeed()) {
            this->_accelY -= _delta * -getAccelerationValue() / 0.5f;
        } else {
            _accelY = -getMaxSpeed();
        }
    } else {
        _accelY = 0;
        // _position.y = _floor.y - 8;
    }

    if (_jumpRequested) {
        float speed = 2.9f;
        float height = 4.5f;

        if (_standingObject.width != 0 && this->_jumpAccelY > 0) {
            _jumpRequested = false;
            _jumpAccelY = 0;
            auto rec = GetCollisionRec(_standingObject, m_rect);

            if (!inWall()) m_rect.y += rec.height;

            //printf("----------------- STOP JUMP LOGIC (%f)\n", rec.height);
        } else {
            if (_jumpAccelY < height) {
                this->_jumpAccelY += _delta * getAccelerationValue() * speed / 3;
                _accelY += -(height - this->_jumpAccelY) * _delta * 70;;

                //printf("%f %f\n", _accelY, this->_jumpAccelY );
            } else {
                _jumpRequested = false;
                _accelY = _jumpAccelY;
                _jumpAccelY = 0;
            }
        }
    }

    if (_accelY == 0 && _jumpAccelY == 0) {
        fixPlayerY();
    }

    return;
}

void Player::fixPlayerY() {
    if (_standingObject.width == 0 || _standingObject.height == 0) return;
    
    if (getRoundedPosition().y < _standingObject.y) {
        int v = getRoundedPosition().y - _standingObject.y < 0;

        //printf("v=%d\n", v);

        if (v < 0) return;

        m_rect.y = _standingObject.y - m_rect.height;
    }
}

bool Player::inWall() {
    auto recs = splitPlayerHitbox2V();

    for (auto [id, reс] : recs) {
        for (auto obj : _objects) {
            auto rec = GetCollisionRec(obj, reс);
        
            if (rec.width > 0) {
                printf("Player::inWall(): id=%s -> return true;\n", id.c_str());
                return true;
            }
        }
    }

    return false;
}

void Player::fixPlayerX() {
    // auto pl = m_rect;

    // // get by sides
    // // left
    // pl.width /= 2;

    // auto rec = GetCollisionRec(_standingObject, pl);
    
    // m_rect.x -= rec.width;

    // pl.x += pl.width;

    // rec = GetCollisionRec(_standingObject, pl);
    
    // m_rect.x += rec.width;

    // float sz = -(pl.x - (_standingObject.x + _standingObject.width));

    // printf("----------------- (%f)\n", sz);
}

void Player::processColliding() {
    if (_standingObject.width != 0) {
        _lastStandingObject = _standingObject;
    }
    _standingObject = {};

    for (auto obj : _objects) {
        Rectangle r1 = obj;
        // r1.y -= 0.75;

        Rectangle r2 = m_rect;

        if (CheckCollisionRecs(r1, r2)) _standingObject = obj;
    }
}

void Player::processXAcceleration() {
    if (_finishRight) {
        if (_accelX > 0) {
            _accelX -= _delta * getStopSpeed();
        } else {
            _accelX = 0;
            _finishRight = false;
        }
    }

    if (_finishLeft) {
        if (_accelX < 0) {
            _accelX += _delta * getStopSpeed();
        } else {
            _accelX = 0;
            _finishLeft = false;
        }
    }

    m_rect.x += _accelX;

    if (inWall()) m_rect.x -= _accelX;
}

void Player::processYAcceleration() {
    _oldPosY = m_rect.y;
    m_rect.y += _accelY + _jumpAccelY;
}

Vector2 Player::getRoundedPosition() {
    Vector2 v;

    v.x = (int)m_rect.x;
    v.y = (int)m_rect.y;

    return v;
}

void Player::setFloor(Rectangle floor) {
    _objects.push_back(floor);
}

void Player::jump(bool hold) {
    printf("JUMP\n");

    if (isFalling()) {
        _scheduledJump = true;
        return;
    }
    if (_jumpRequested) {
        if (hold) _scheduledJump = true;
        return;
    }

    _jumpRequested = true;

    fixPlayerY();
}

bool Player::isFalling() {
    for (auto obj : _objects) {
        Rectangle r1 = obj;
        r1.y -= 1;

        Rectangle r2 = m_rect;

        Rectangle col = GetCollisionRec(r1, r2);

        if (CheckCollisionRecs(r1, r2) && col.width > 0) return false;
    }

    return true;
}

void Player::resetFloors() {
    _objects.clear();
}

void Player::setFloor(std::vector<Rectangle> floors) {
    _objects.insert(_objects.end(), floors.begin(), floors.end());
}

std::unordered_map<std::string, Rectangle> Player::splitPlayerHitbox4() {
    float width = m_rect.width / 2.f;
    float height = m_rect.height / 2.f;

    std::unordered_map<std::string, Rectangle> map = {};

    Rectangle topRightCorner = {
        m_rect.x + width, m_rect.y,
        width, height
    };
    Rectangle topLeftCorner = {
        m_rect.x, m_rect.y,
        width, height
    };
    Rectangle bottomRightCorner = {
        m_rect.x + width, m_rect.y + height,
        width, height
    };
    Rectangle bottomLeftCorner = {
        m_rect.x, m_rect.y + height,
        width, height
    };

    map["top-right-corner"] = topRightCorner;
    map["top-left-corner"] = topLeftCorner;
    map["bottom-right-corner"] = bottomRightCorner;
    map["bottom-left-corner"] = bottomLeftCorner;

    return map;
}

std::unordered_map<std::string, Rectangle> Player::splitPlayerHitbox2V() {
    float width = m_rect.width / 2.f;
    float height = m_rect.height / 2.f;

    std::unordered_map<std::string, Rectangle> map = {};

    float padding = 1.f;
    
    Rectangle leftSide = {
        m_rect.x, m_rect.y + padding,
        width, height * 2.f - (padding * 2.f)
    };
    Rectangle rightSide = {
        m_rect.x + width, m_rect.y + padding,
        width, height * 2.f - (padding * 2.f)
    };

    map["left-side"] = leftSide;
    map["right-side"] = rightSide;

    return map;
}

std::unordered_map<std::string, Rectangle> Player::splitPlayerHitbox2H(bool precise) {
    float width = m_rect.width / 2.f;
    float height = m_rect.height / 2.f;

    std::unordered_map<std::string, Rectangle> map = {};

    float padding = 2.f;

    if (precise) {
        padding = 0.f;
    }
    
    Rectangle leftSide = {
        m_rect.x + padding, m_rect.y,
        width * 2.f - (padding * 2.f), height
    };
    Rectangle rightSide = {
        m_rect.x + padding, m_rect.y + height,
        width * 2.f - (padding * 2.f), height
    };

    map["top"] = leftSide;
    map["bottom"] = rightSide;

    return map;
}

bool Player::reachedCeiling() {
    Rectangle topRect = splitPlayerHitbox2H(true)["top"];

    for (auto obj : _objects) {
        auto rec = GetCollisionRec(obj, topRect);
        
        if (rec.width > 0) {
            _reachedCeilingObject = obj;
            _currentCollisionBox = rec;
            // //printf("Player::reachedCeiling(): id=%s -> return true;\n", id.c_str());
            return true;
        }
    }

    return false;
}

Rectangle Player::roundRectangle(Rectangle rec) {
    Rectangle new_rect = {
        (int)rec.x,
        (int)rec.y,
        (int)rec.width,
        (int)rec.height
    };

    return new_rect;
    // return rec;
}

bool Player::wallVeryClose() {
    m_rect.x -= 2.f;
    if (inWall()) {
        m_rect.x += 2.f;
        return true;
    }

    m_rect.x += 4.f;
    if (inWall()) {
        m_rect.x -= 2.f;
        return true;
    }

    return false;
}
