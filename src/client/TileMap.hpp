#pragma once

#include <raylib.h>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <filesystem>

class TileMap {
protected:
    Texture2D m_map;
    Vector2 m_tileSize = {0, 0};
    std::map<uint16_t, Texture2D> m_cachedTextures; 

public:
    TileMap(std::filesystem::path const& path, Vector2 tileSize);
    ~TileMap();

    Vector2 getPositionByIndex(uint16_t index);
    Rectangle getRectForTile(Vector2 position);
    Rectangle getRectForTile(uint16_t index);

    void drawTile(uint16_t index, Vector2 position, Color color = WHITE, bool flipedX = false);
    void drawTilePro(uint16_t index, Rectangle dest, Color color = WHITE, bool flipedX = false);

    Texture2D getTextureOfTileCached(uint16_t index);
};