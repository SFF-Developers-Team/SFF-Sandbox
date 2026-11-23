#include <Tilemap.hpp>

#include <stdexcept>

Tilemap::Tilemap(raylib::Texture2D& texture, int columns, int rows)
    : m_texture(texture), m_columns(columns), m_rows(rows),
      m_tileWidth(columns > 0 ? float(texture.width) / columns : 0.f),
      m_tileHeight(rows > 0 ? float(texture.height) / rows : 0.f) {
    if (columns <= 0 || rows <= 0 || texture.width % columns || texture.height % rows) {
        throw std::invalid_argument("Invalid tilemap grid dimensions");
    }
}

raylib::Rectangle Tilemap::GetRectForTile(int index) const {
    if (index < 0 || index >= m_columns * m_rows) {
        throw std::out_of_range("Tilemap sprite index is out of range");
    }
    return {float(index % m_columns) * m_tileWidth,
            float(index / m_columns) * m_tileHeight,
            m_tileWidth, m_tileHeight};
}

void Tilemap::DrawTile(int index, raylib::Vector2 position, raylib::Color color) const {
    m_texture.Draw(GetRectForTile(index), position, color);
}

void Tilemap::DrawTile(int index, raylib::Rectangle dest, raylib::Color color,
                       float rotation, raylib::Vector2 origin) const {
    m_texture.Draw(GetRectForTile(index), dest, origin, rotation, color);
}
