#pragma once

#include <raylib.h>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <Types.hpp>
#include <filesystem>

class TileMap {
protected:
    Texture2D m_map;
    Vec2i m_tileSize;
    std::map<uint16_t, Texture2D> m_cachedTextures;

public:
    TileMap(std::filesystem::path const& path, Vec2i tileSize);
    ~TileMap();

    Vec2i getPositionByIndex(uint16_t index);
    Rectangle getRectForTile(Vec2i position);
    Rectangle getRectForTile(uint16_t index);
    Texture2D& getTextureOfTileCached(uint16_t index);
    Texture2D& getMap();
};