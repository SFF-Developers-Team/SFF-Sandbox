#include "animation.h"
#include "raymath.h"
#include "utils.h"
#include "textures.h"
#include "skin.h"

#define ANIM_CLAMP(var, min, max) if (var < min) var = min; if (var > max) var = min;

void EntityAnimation_Tick(EntityAnimation* anim) {
    if (anim->ticks > anim->ticksPerFrame) {
        anim->ticks = 0;
        anim->frame++;
    }

    switch (anim->type) {
        case ENTITY_HIT:
            if (anim->frame < 9) anim->frame = 9;
            if (anim->frame >= 13) {
                anim->type = ENTITY_IDLE;
                anim->frame = 9;
            }
            break;
        case ENTITY_IDLE: anim->frame = 0; break;
        case ENTITY_WALK: ANIM_CLAMP(anim->frame, 1, 5) break;
        case ENTITY_SNEAK: ANIM_CLAMP(anim->frame, 6, 7); break;
        case ENTITY_JUMP: anim->frame = 8; break;
        case ENTITY_HURT: anim->frame = 14; break;
        case ENTITY_SIT: anim->frame = 15; break;
        case ENTITY_CART: anim->frame = 16; break;
    }

    anim->ticks++;
}

void EntityAnimation_Draw(EntityAnimation* anim, Vector2 frameSize, Rectangle dest) {
    Rectangle source = {anim->frame * frameSize.x, 0.0f, frameSize.x, frameSize.y};

    if (anim->direction != DIRECTION_LEFT) source.width *= -1;

    DrawTexturePro(anim->texture, source, dest, Vector2Zero(), 0.0f, WHITE);
}