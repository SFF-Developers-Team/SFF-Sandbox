#pragma once

#include <Rectangle.hpp>
#include <Texture.hpp>
#include <Vector2.hpp>

class Tilemap {
public:
    // columns and rows are the number of sprites in the atlas.
    Tilemap(raylib::Texture2D& texture, int columns, int rows);

    void DrawTile(int index, raylib::Vector2 position, raylib::Color color = raylib::Color::White()) const;
    void DrawTile(int index, raylib::Rectangle dest, raylib::Color color = raylib::Color::White(),
                  float rotation = 0.f, raylib::Vector2 origin = raylib::Vector2::Zero()) const;

private:
    [[nodiscard]] raylib::Rectangle GetRectForTile(int index) const;

    raylib::Texture2D& m_texture;
    int m_columns;
    int m_rows;
    float m_tileWidth;
    float m_tileHeight;
};
