#pragma once
#include "Rectangle.hpp"
#include "Types.hpp"
#include <Color.hpp>
#include <Texture.hpp>
#include <Vector2.hpp>

class Tilemap {
public:
    Tilemap(raylib::Texture2D& texture, int tileCountX, int tileCountY);

    void DrawTile(int index, RVector2 position, raylib::Color color = WHITE);
    void DrawTile(int index, RRectangle dest, raylib::Color color = WHITE);

private:
    inline RRectangle GetRectForTile(int index);

    raylib::Texture2D& m_texture;
    Vector2i m_tileSize;
};