#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "blocks.h"
#include "defines.h"
#include <stdint.h>
#include "utils.h"

#define PACK_INDEX(x, y, width) (y * width + x)

typedef enum {
    WORLD_SMALL,
    WORLD_MEDIUM,
    WORLD_LARGE
} WorldSize;

typedef enum {
    WORLD_DEFAULT,
    WORLD_FLAT
} WorldType;

// WARNING: CreateWorld screen can set world metadata but not generate data, then JoinWorld screen should generate world
extern struct WorldData {
    int width;
    int height;

    int spawnX;
    int spawnY;

    const char* name;
    uint64_t seed;
    WorldSize size;
    WorldType type;
    
    uint8_t* data;

    bool loaded;
} World;

Texture2D World_GeneratePreview(int seed, int worldType, int worldSize);

// WARNING: This function will OVERWRITE current loaded world
int World_Generate(WorldType wType, WorldSize wSize, int seed);
void World_GetSize(WorldSize worldSize, int* w, int* h);

void World_Reset();
int World_Save();
int World_Load(const char* name);

static inline void World_SetBlock(int x, int y, Block id) {
    World.data[PACK_INDEX(x, y, World.width)] = id;
}

static inline Block World_GetBlock(int x, int y) {
    int w = 0;
    int h = 0;

    int convX = MAX(0, x);
    int convY = MAX(0, y);

    World_GetSize(World.size, &w, &h);

    if (convX < w || convY < h) {
        return World.data[PACK_INDEX(convX, convY, World.width)];
    } else {
        return 0;
    }
}
#endif