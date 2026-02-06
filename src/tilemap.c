#include "tilemap.h"
#include "raymath.h"
#include "utils.h"
#include "defines.h"
#include <stdlib.h>
#include <string.h>

struct TilemapEntry* tilemapsHead;
struct TilemapEntry* tilemapsTail;

int LoadTilemapEntry(struct TilemapEntry* map) {
    LINKEDLIST_APPEND(map, tilemapsHead, tilemapsTail);
    return 0;
}

Tilemap* FindTilemap(const char* name) {
    struct TilemapEntry* entry;

	for (entry = tilemapsHead; entry; entry = entry->next) {
		if (!TextIsEqual(name, entry->name)) continue;

		return &entry->tilemap;
	}

    TraceLog(LOG_WARNING, "TILEMAP: Can't find tilemap %s", name);

	return NULL;
}

void DrawTile(Tilemap tilemap, int index, int posX, int posY, Color tint) {
    DrawTileEx(tilemap, index, (Vector2){(float)posX, (float)posY}, 0.0f, 1.0f, tint);
}

void DrawTileV(Tilemap tilemap, int index, Vector2 position, Color tint) {
    DrawTileEx(tilemap, index, position, 0.0f, 1.0f, tint);
}

void DrawTileEx(Tilemap tilemap, int index, Vector2 pos, float rotation, float scale, Color tint) {
    Vector2 tileSize = (Vector2){tilemap.texture.width / tilemap.tilesH, tilemap.texture.height / tilemap.tilesV};

    DrawTilePro(
        tilemap, 
        index, 
        (Rectangle){pos.x, pos.y, tileSize.x * scale, tileSize.y * scale}, 
        Vector2Zero(), 
        rotation, 
        tint
    );
}

void DrawTileRec(Tilemap tilemap, int index, Vector2 position, Color tint) {
    Vector2 tileSize = (Vector2){tilemap.texture.width / tilemap.tilesH, tilemap.texture.height / tilemap.tilesV};

    DrawTilePro(tilemap, index, (Rectangle){position.x, position.y, tileSize.x, tileSize.y}, Vector2Zero(), 0.0f, tint);
}

void DrawTilePro(Tilemap tilemap, int index, Rectangle dest, Vector2 origin, float rotation, Color tint) {
    Vector2 tileSize = (Vector2){tilemap.texture.width / tilemap.tilesH, tilemap.texture.height / tilemap.tilesV};
    
    Rectangle src = (Rectangle){
        index % tilemap.tilesV * tileSize.x,
        index / tilemap.tilesH * tileSize.y,
        tileSize.x,
        tileSize.y
    };

    DrawTexturePro(tilemap.texture, src, dest, origin, rotation, tint);
}