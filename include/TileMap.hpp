#pragma once

#include <raylib.h>
#include <vector>
#include <string>
#include <cmath>
class TileMap {
protected:
    Texture2D m_map;
    Vector2 m_tileSize = {0, 0};

public:
    TileMap(std::string path, Vector2 tileSize);
    ~TileMap();

    Vector2 getPositionByIndex(int index);
    Rectangle getRectForTile(Vector2 position);
    void drawTile(Vector2 tile, Vector2 position, Color color = WHITE, bool flipedX = false);
    void drawTilePro(Vector2 tile, Rectangle dest, Color color = WHITE, bool flipedX = false);

    Texture2D loadTile(Vector2 tile);
};