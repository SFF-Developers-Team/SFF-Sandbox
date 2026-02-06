#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "raylib.h"

#define PLAYER_FRAME (Vector2){SKIN_FRAME_WIDTH, SKIN_FRAME_HEIGHT}

typedef enum {
    ENTITY_IDLE,
    ENTITY_WALK,
    ENTITY_SNEAK,
    ENTITY_JUMP,
    ENTITY_HIT,
    ENTITY_HURT,
    ENTITY_SIT,
    ENTITY_CART
} EntityAnimationType;

typedef enum {
    DIRECTION_LEFT,
    DIRECTION_RIGHT
} Direction;

typedef struct {
    Texture2D texture;
    int frame;
    int ticks;
    int ticksPerFrame;
    EntityAnimationType type;

    Direction direction : 1;
} EntityAnimation;

void EntityAnimation_Tick(EntityAnimation* texture);
void EntityAnimation_Draw(EntityAnimation* texture, Vector2 frameSize, Rectangle dest);
#endif