#ifndef TEXTURES_H
#define TEXTURES_H
#include "raylib.h"

struct TextureEntry;

struct TextureEntry {
    const char* name;
    Texture2D texture;
    struct TextureEntry* next;
};

extern struct TextureEntry* texturesHead;
extern struct TextureEntry* texturesTail;

int LoadTextureEntry(struct TextureEntry* tex);
Texture2D* FindTexture(const char* name);

#endif