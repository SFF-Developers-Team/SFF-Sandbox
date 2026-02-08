#include "world.h"
#include "stb_perlin.h"
#include <math.h>
#include "stream.h"
#include "files.h"
#include "defines.h"
#include <string.h>
#include "error.h"

#define PREVIEW_IMAGE_WIDTH 128
#define PREVIEW_IMAGE_HEIGHT 64

#define PREVIEW_GRASS_COLOR DARKGREEN
#define PREVIEW_DIRT_COLOR  (Color){59, 32, 0, 255}
#define PREVIEW_STONE_COLOR GRAY

struct WorldData World;

void World_GetSize(WorldSize worldSize, int* w, int* h) {
    switch (worldSize) {
        case WORLD_SMALL: 
            *w = 2000;
            *h = 500;
            break;
        case WORLD_MEDIUM: 
            *w = 3000;
            *h = 1000;
            break;
        case WORLD_LARGE: 
            *w = 6000;
            *h = 2000;
            break;
    }
}

// yOff = -1: search for surface
void World_GeneratePart(WorldType wType, WorldSize wSize, int seed, int w, int h, int xOff, int yOff, uint8_t* out) {
    int worldW, worldH;
    World_GetSize(wSize, &worldW, &worldH);
    
    SetRandomSeed(seed);

    switch (wType) {
        case WORLD_DEFAULT: {
            int surfaceDepth = worldH / 20;
            int terrainDepth = worldH * 2 / 3;

            for (int x = 0; x < w; x++) {
                int realX = x + xOff;

                int surface = worldH - (stb_perlin_noise3_seed(realX * 0.01f, 0.0f, 0.0f, 0, 0, 0, seed) * surfaceDepth + terrainDepth);
                if (yOff == -1) {
                    yOff = surface - h / 2;
                }

                surface -= yOff;

                *(uint8_t*)(out + PACK_INDEX(x, surface, w)) = GRASS;

                for (int y = surface + 1; y < h; y++) {
                    if (y - surface < GetRandomValue(5, 7)) {
                        *(uint8_t*)(out + PACK_INDEX(x, y, w)) = DIRT;
                        continue;
                    }

                    *(uint8_t*)(out + PACK_INDEX(x, y, w)) = STONE;
                }
            }
            

            break;
        }

        case WORLD_FLAT: {
            int terrainDepth = worldH * 2 / 3;

            for (int x = 0; x < w; x++) {
                int surface = worldH - terrainDepth;
                if (yOff == -1) {
                    yOff = surface - h / 2;
                }

                surface -= yOff;

                *(uint8_t*)(out + PACK_INDEX(x, surface, w)) = GRASS;

                for (int y = surface + 1; y < h; y++) {
                    if (y - surface < GetRandomValue(5, 7)) {
                        *(uint8_t*)(out + PACK_INDEX(x, y, w)) = DIRT;
                        continue;
                    }

                    *(uint8_t*)(out + PACK_INDEX(x, y, w)) = STONE;
                }
            }
        }
    }    
}

Texture2D World_GeneratePreview(int seed, int worldType, int worldSize) {
    static Image previewImage = { 0 };
    static Texture2D previewTexture = { 0 };

    int w = PREVIEW_IMAGE_WIDTH;
    int h = PREVIEW_IMAGE_HEIGHT;

    if (!previewImage.data) {
        previewImage = GenImageColor(w, h, SKYBLUE);
    } else {
        ImageDrawRectangle(&previewImage, 0, 0, w, h, SKYBLUE);
    }

    int worldW, worldH;
    World_GetSize(worldSize, &worldW, &worldH);

    int xOff = worldW * 2 / 3;
    int yOff = -1;

    uint8_t previewData[PREVIEW_IMAGE_WIDTH * PREVIEW_IMAGE_HEIGHT];
    memset(previewData, 0, sizeof(previewData));

    World_GeneratePart(worldType, worldSize, seed, w, h, xOff, yOff, previewData);

    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Color blockColor;

            switch (previewData[PACK_INDEX(x, y, w)]) {
                case AIR: continue;
                case GRASS: blockColor = PREVIEW_GRASS_COLOR; break;
                case DIRT: blockColor = PREVIEW_DIRT_COLOR; break;
                case STONE: blockColor = PREVIEW_STONE_COLOR; break;
            }

            ImageDrawPixel(&previewImage, x, y, blockColor);
        }
    }

    if (previewTexture.id != 0) 
        UpdateTexture(previewTexture, previewImage.data);
    else
        previewTexture = LoadTextureFromImage(previewImage);

    return previewTexture;
}

int World_Generate(WorldType wType, WorldSize wSize, int seed) {
    int worldW, worldH;
    World_GetSize(wSize, &worldW, &worldH);

    World.width = worldW;
    World.height = worldH;
    World.size = wSize;
    World.seed = seed;
    World.type = wType;
    World.data = MemAlloc(worldW * worldH);

    if (!World.data) {
        TraceLog(LOG_ERROR, "WORLD: Failed to allocate world");
        return RES_OUT_OF_MEMORY;
    }

    World.spawnX = GetRandomValue(0, World.width);

    World_GeneratePart(wType, wSize, seed, worldW, worldH, 0, 0, World.data);

    for (int y = 0; y < worldH; y++) {
        if (World.data[PACK_INDEX(World.spawnX, y, World.width)]) {
            World.spawnY = y - 2;
            break;
        }
    }

    return 0;
}

int World_Save() {
    char* path = GetDataSubdirectory("saves/");
    strcat(path, World.name);
    strcat(path, ".dat");

    struct Stream stream = { 0 };

    int res = Stream_CreateFile(&stream, path);

    if (res != 0) return -1;

    Stream_Write16(&stream, World.width);
    Stream_Write16(&stream, World.height);
    
    Stream_Write16(&stream, World.spawnX);
    Stream_Write16(&stream, World.spawnY);
    
    Stream_Write64(&stream, World.seed);

    Stream_Write8(&stream, World.size);
    Stream_Write8(&stream, World.type);
    
    stream.Write(&stream, World.data, World.width * World.height);
    stream.Close(&stream);

    return 0;
}

int World_Load(const char* name) {
    char* path = GetDataSubdirectory("saves/");
    strcat(path, name);
    strcat(path, ".dat");

    struct Stream stream = { 0 };
    int res = Stream_OpenFile(&stream, path);

    if (res != 0) return res;

    World.width = Stream_Read16(&stream);
    World.height = Stream_Read16(&stream);
    
    World.spawnX = Stream_Read16(&stream);
    World.spawnY = Stream_Read16(&stream);
    
    World.seed = Stream_Read64(&stream);

    World.size = Stream_Read8(&stream);
    World.type = Stream_Read8(&stream);

    World.data = MemAlloc(World.width * World.height);

    World.name = name;

    stream.Read(&stream, World.data, World.width * World.height);
    stream.Close(&stream);

    World.loaded = true;
    return 0;
}

void World_Reset() {
    World.loaded = false;
    World.width = 0;
    World.height = 0;
    World.size = 0;
    World.seed = 0;
    World.type = 0;
    MemFree(World.data);
}