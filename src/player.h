#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "world.h"
#include "animation.h"

typedef enum {
    CONTROL_KEYBOARD_MOUSE,
    CONTROL_GAMEPAD
} ControlType;

typedef struct {
    Vector2 position;
    Vector2 prevPosition;

    Vector2 speed;
    bool onGround;

    EntityAnimation animation;
} Player;

typedef struct {
    Player player;
    Camera2D camera;
    BlockPosisiton lookAt;

    // controls
    ControlType controlType;
    int gamepad;
    bool replaceBlockMode;
} LocalPlayer;

void Player_Tick(Player* player);
void Player_Draw(Player* player);
Rectangle Player_GetCollisionRect(Player* player);
Rectangle Player_GetVisibleRect(Player* player);

void LocalPlayer_UpdateControls(LocalPlayer* player);
#endif