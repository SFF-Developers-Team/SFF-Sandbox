#ifndef TILEMAP_H
#define TILEMAP_H

#include "raylib.h"

typedef struct {
    Texture2D texture;
    int tilesV;
    int tilesH;
} Tilemap;

struct TilemapEntry;
struct TilemapEntry {
    const char* name;
    Tilemap tilemap;
    struct TilemapEntry* next;
};

extern struct TilemapEntry* tilemapsHead;
extern struct TilemapEntry* tilemapsTail;

int LoadTilemapEntry(struct TilemapEntry* map);
Tilemap* FindTilemap(const char* name);

void DrawTile(Tilemap tilemap, int index, int posX, int posY, Color tint);
void DrawTileV(Tilemap tilemap, int index, Vector2 position, Color tint);
void DrawTileEx(Tilemap tilemap, int index, Vector2 position, float rotation, float scale, Color tint);
void DrawTileRec(Tilemap tilemap, int index, Vector2 position, Color tint);
void DrawTilePro(Tilemap tilemap, int index, Rectangle dest, Vector2 origin, float rotation, Color tint);

#endif