#pragma once

#include <raylib.h>
#include <vector>
#include <SerializedObject.hpp>
#include <unordered_map>
#include <string>

class Player : public SerializedObject {
private:
    Rectangle m_rect;
    Texture2D m_texture;

    Vector2 m_speed = {0.0f, 0.0f};
    char m_direction = 1;
    bool m_canJump = false;
    bool m_fly = false;

    std::vector<Rectangle> _objects = {};

    float _accelX = 0;
        
    float _accelY = 0;
    float _jumpAccelY = 0;
    
    Rectangle _standingObject = {};
    Rectangle _lastStandingObject = {};

    float _delta = 1.f / 60.f;

    bool _lookingToRight = true;

    bool _finishRight = false;
    bool _finishLeft = false;

    bool _jumpRequested = false;

    float _oldPosY = 0.f; 

    bool _scheduledJump = false;

    Rectangle _reachedCeilingObject;
    Rectangle _currentCollisionBox;

    void processXAcceleration();
    void processYAcceleration();

    void processGravity();

    void processColliding();

    void fixPlayerY();
    void fixPlayerX();

    bool inWall();

    virtual float getMaxSpeed();
    virtual float getStopSpeed();
    virtual float getAccelerationValue();

    static Rectangle roundRectangle(Rectangle rec);
public:
    Camera2D camera;

    Player();
 
    void update(std::vector<Rectangle> envHitboxes); 
    void draw();
    void updateCamera();

    inline Vector2 getPosition() {
        return Vector2 {m_rect.x, m_rect.y};
    }

    SObject encodeObject() override;
    int decodeObject(SObject &s) override;

    virtual void moveRight();
    virtual void moveLeft();

    virtual void releaseMovementRight();
    virtual void releaseMovementLeft();

    bool isFalling();

    void setFloor(Rectangle floor);
    void setFloor(std::vector<Rectangle> floors);
    void resetFloors();

    virtual void processMovement();

    virtual void jump(bool hold);

    Vector2 getRoundedPosition();

    std::unordered_map<std::string, Rectangle> splitPlayerHitbox4();
    std::unordered_map<std::string, Rectangle> splitPlayerHitbox2V();
    std::unordered_map<std::string, Rectangle> splitPlayerHitbox2H(bool precise = false);

    bool reachedCeiling();

    bool wallVeryClose();
};