#ifndef SKIN_H
#define SKIN_H

#define SKIN_FRAME_WIDTH 16.f
#define SKIN_FRAME_HEIGHT 22.f

#include "raylib.h"
#include "animation.h"

typedef struct Skin {
    int id;
    Texture2D texture;
} Skin;

struct SkinEntry;

struct SkinEntry {
    Skin skin;
    struct SkinEntry* next;
};

extern struct SkinEntry* loadedSkinHead;
extern struct SkinEntry* loadedSkinTail;

extern Skin defaultSkin;
extern Skin* currentSkin;

Texture2D LoadSkinTexture(const char* path);

void Skin_Load(struct SkinEntry* entry);

// 0 for default skin
Skin* Skin_Find(int id);

void Skin_Draw(Skin* skin, Rectangle dest);
#endif