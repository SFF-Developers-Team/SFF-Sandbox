#include "Tilemap.hpp"
#include "Vector2.hpp"
#include <Rectangle.hpp>
#include <raylib.h>

Tilemap::Tilemap(raylib::Texture2D& texture, int tileCountX, int tileCountY) 
    : m_texture(texture), m_tileSize(texture.GetWidth() / tileCountX, texture.GetHeight() / tileCountY) {}

RRectangle Tilemap::GetRectForTile(int index) {
    float const epsilon = 0.5f / m_texture.GetWidth();

    RVector2 position(
        index % (m_texture.GetWidth() / m_tileSize.x), 
        index / (m_texture.GetHeight() / m_tileSize.y)
    );

    RRectangle r(
        position.x * m_tileSize.x + epsilon,
        position.y * m_tileSize.y + epsilon,
        m_tileSize.x - 2 * epsilon,
        m_tileSize.y - 2 * epsilon
    );

    return r;
}

void Tilemap::DrawTile(int index, RVector2 position) {
    m_texture.Draw(GetRectForTile(index), position);
}

void Tilemap::DrawTile(int index, RRectangle dest) {
    m_texture.Draw(GetRectForTile(index), dest);
}