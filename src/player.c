#include "player.h"
#include "collisions.h"
#include <stdbool.h>
#include <math.h>
#include "raymath.h"
#include "world.h"
#include "skin.h"
#include "utils.h"

#define PLAYER_BOX_WIDTH 1.f
#define PLAYER_BOX_HEIGHT 1.375f

void Player_Tick(Player* player) {
    player->speed.y += 0.02f;

    Vector2 prevSpeed = player->speed;
    Rectangle playerBox = Player_GetCollisionRect(player);

    player->prevPosition = player->position;

    // Find blocks around player
    Vector2 blocksAroundArr[10] = { 0 };
    Rectangle playerBoxEx = ExpandRectangle(playerBox, player->speed.x, player->speed.y);

    playerBoxEx.x = Clamp(playerBoxEx.x, 1.0f, World.width - playerBoxEx.width);
    playerBoxEx.y = Clamp(playerBoxEx.y, 1.0f, World.height - playerBoxEx.height);

    int blocksAroundCount = 0;
    for (int x = (int)playerBoxEx.x - 1; x <= (int)(playerBoxEx.x + playerBoxEx.width); x++) {
        for (int y = (int)playerBoxEx.y - 1; y <= (int)(playerBoxEx.y + playerBoxEx.height); y++) {
            if (World_GetBlock(x, y)) {
                blocksAroundArr[blocksAroundCount] = (Vector2){(float)x, (float)y};
                blocksAroundCount++;
            }

            if (blocksAroundCount >= 20) break;
        }
    }

    // Check for X collision
    float x = player->speed.x;
    for (int i = 0; i < blocksAroundCount; i++) {
        x = Collision_ClipX((Rectangle){blocksAroundArr[i].x, blocksAroundArr[i].y, 1.0f, 1.0f}, playerBox, x);
    }
    player->position.x += x;

    // Check for Y collision
    float y = player->speed.y;
    for (int i = 0; i < blocksAroundCount; i++) {
        y = Collision_ClipY((Rectangle){blocksAroundArr[i].x, blocksAroundArr[i].y, 1.0f, 1.0f}, playerBox, y);
    }
    player->position.y += y;

    // Update on ground state
    player->onGround = prevSpeed.y != y && prevSpeed.y > 0.0f;

    // Stop motion on collision
    if (prevSpeed.x != x) player->speed.x = 0.0f;
    if (prevSpeed.y != y) player->speed.y = 0.0f;

    // Slow down player
    player->speed.x *= 0.91f;
    player->speed.y *= 0.98f;
    
    // Slow down more if player on ground
    if (player->onGround) {
        player->speed.x *= 0.7f;
        player->speed.y *= 0.7f;
    }

    // Reset animation TPF
    player->animation.ticksPerFrame = 5;

    if (player->speed.x != 0.0f) {
        player->animation.type = ENTITY_WALK;
        player->animation.direction = (player->speed.x < 0.0f)? DIRECTION_LEFT : DIRECTION_RIGHT;
        player->animation.ticksPerFrame = (int)(1.0f / fabsf(player->speed.x)); // Animation speed depends on player speed
    }

    // Player too slow 
    // If player hits smth, hit animation should not be overriden by idle animation
    if (player->speed.x > -0.01f && player->speed.x < 0.01f && player->animation.type != ENTITY_HIT) {
        player->animation.type = ENTITY_IDLE;
    }

    // jump/fly
    if (player->speed.y != 0.0f) {
        player->animation.type = ENTITY_JUMP;
    }

    EntityAnimation_Tick(&player->animation);
}

void Player_Draw(Player* player) {
    EntityAnimation_Draw(&player->animation, PLAYER_FRAME, Player_GetVisibleRect(player));
}

Rectangle Player_GetCollisionRect(Player* player) {
    return (Rectangle){
        player->position.x + PLAYER_BOX_WIDTH * 0.1f, 
        player->position.y + PLAYER_BOX_WIDTH * 0.1f, 
        PLAYER_BOX_WIDTH * 0.8f, 
        PLAYER_BOX_HEIGHT * 0.9f
    };
}

Rectangle Player_GetVisibleRect(Player* player) {
    return (Rectangle){player->position.x, player->position.y, PLAYER_BOX_WIDTH, PLAYER_BOX_HEIGHT};
}

#define REACH_DISTANCE 6.f

void LocalPlayer_UpdateControls(LocalPlayer* localPlayer) {
    switch (localPlayer->controlType) {
        case CONTROL_KEYBOARD_MOUSE: {
            Vector2 fLookAt = GetScreenToWorld2D(GetMousePosition(), localPlayer->camera);

            int bx = (int)fLookAt.x;
            int by = (int)fLookAt.y;
            
            bool validLook = (
                !CheckCollisionRecs((Rectangle){bx, by, 1.0f, 1.0f}, Player_GetCollisionRect(&localPlayer->player)) && 
                Vector2Distance(localPlayer->player.position, fLookAt) < REACH_DISTANCE + 1.f
            );

            localPlayer->replaceBlockMode = IsKeyDown(KEY_LEFT_CONTROL);
            localPlayer->lookAt = (validLook)? (BlockPosisiton){bx, by} : INVALID_BLOCK_POSITION;

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && validLook) {
                World_SetBlock(localPlayer->lookAt.x, localPlayer->lookAt.y, AIR);
                localPlayer->player.animation.type = ENTITY_HIT;
            }

            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && validLook && (!World_GetBlock(bx, by) || localPlayer->replaceBlockMode)) {
                World_SetBlock(localPlayer->lookAt.x, localPlayer->lookAt.y, localPlayer->player.currentBlock);
                localPlayer->player.animation.type = ENTITY_HIT;
            }

            float horizontal = 0.0f;

            if ((IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE) ||  IsKeyDown(KEY_UP)) && localPlayer->player.onGround) {
                localPlayer->player.speed.y = -0.3f;
            }

            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
                horizontal -= 0.2f;
            }

            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
                horizontal += 0.2f;
            }
            
            // Block choose
            for (int i = 0; i < 13; i++) {
                if (IsKeyDown(KEY_ONE + i)) {
                    localPlayer->player.currentBlock = i + 1;
                }
            }
            
            localPlayer->player.speed.x = horizontal;
            break;
        }
        
        case CONTROL_GAMEPAD: {
            float rightX = GetGamepadAxisMovement(localPlayer->gamepad, GAMEPAD_AXIS_RIGHT_X);
            float rightY = GetGamepadAxisMovement(localPlayer->gamepad, GAMEPAD_AXIS_RIGHT_Y);
            const float deadzone = 0.2f;

            Vector2 dir = (Vector2){rightX, rightY};

            bool validLook = false;
            int bx, by;

            // Attempt to implement smart cursor
            for (float t = 0; t < REACH_DISTANCE; t += 0.1f) {
                Vector2 fLookAt = Vector2Add(localPlayer->player.position, (Vector2){
                    dir.x * t, 
                    dir.y * t
                });

                bx = (int)fLookAt.x;
                by = (int)fLookAt.y;

                validLook = (
                    !CheckCollisionRecs((Rectangle){bx, by, 1.0f, 1.0f}, Player_GetCollisionRect(&localPlayer->player)) //&& 
                    //Vector2Distance(localPlayer->player.position, fLookAt) < REACH_DISTANCE
                );

                localPlayer->lookAt = (validLook)? (BlockPosisiton){bx, by} : INVALID_BLOCK_POSITION;
                
                if (World_GetBlock(bx, by)) break;
            }

            if (IsGamepadButtonPressed(localPlayer->gamepad, GAMEPAD_BUTTON_RIGHT_THUMB)) {
                localPlayer->replaceBlockMode ^= 1;
            }

            bool canPlaceBlock = (!World_GetBlock(bx, by) || localPlayer->replaceBlockMode);


            if (GetGamepadAxisMovement(localPlayer->gamepad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.0f && validLook) {
                World_SetBlock(localPlayer->lookAt.x, localPlayer->lookAt.y, AIR);
                localPlayer->player.animation.type = ENTITY_HIT;
            }

            if (GetGamepadAxisMovement(localPlayer->gamepad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.0f && validLook && canPlaceBlock) {
                World_SetBlock(localPlayer->lookAt.x, localPlayer->lookAt.y, localPlayer->player.currentBlock);
                localPlayer->player.animation.type = ENTITY_HIT;
            }
            
            // Block select
            if (IsGamepadButtonPressed(localPlayer->gamepad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                localPlayer->player.currentBlock++;

                if (localPlayer->player.currentBlock >= 17) {
                    localPlayer->player.currentBlock = GRASS;
                } 
            }

            if (IsGamepadButtonPressed(localPlayer->gamepad, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
                localPlayer->player.currentBlock--;

                if (localPlayer->player.currentBlock < 1) {
                    localPlayer->player.currentBlock = CHEST;
                } 
            }

            if (IsGamepadButtonPressed(localPlayer->gamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && localPlayer->player.onGround) {
                localPlayer->player.speed.y = -0.3f;
            }

            float leftX = GetGamepadAxisMovement(localPlayer->gamepad, GAMEPAD_AXIS_LEFT_X);
            float leftY = GetGamepadAxisMovement(localPlayer->gamepad, GAMEPAD_AXIS_LEFT_Y);
            
            if (leftY < -0.5f && localPlayer->player.onGround) {
                localPlayer->player.speed.y = -0.3f;
            }

            if (leftX > deadzone || leftX < -deadzone) {
                localPlayer->player.speed.x = leftX * 0.2f;
            }

            break;
        }
    }
}