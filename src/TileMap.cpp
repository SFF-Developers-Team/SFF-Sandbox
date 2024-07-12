#include <TileMap.hpp>

TileMap::TileMap(std::string path, Vector2 v) {
    this->m_map = LoadTexture(path.c_str());
    this->m_tileSize = v;
}

TileMap::~TileMap() {
    UnloadTexture(this->m_map);
}

Vector2 TileMap::getPositionByIndex(int index) {
    return Vector2 {floorf(index % (int)(m_map.width / m_tileSize.x)), floorf(index / (m_map.height / m_tileSize.y))};
}

Rectangle TileMap::getRectForTile(Vector2 position) {
    Rectangle r;

    r.width = m_tileSize.x;
    r.height = m_tileSize.y;
    r.x = floor(position.x * m_tileSize.x);
    r.y = floor(position.y * m_tileSize.y);

    return r;
}

void TileMap::drawTile(Vector2 tile, Vector2 position, Color color, bool flipedX) {
    auto rect = getRectForTile(tile);
    
    if (flipedX) {
        rect.width *= -1;
    }

    DrawTextureRec(m_map, rect, position, color);
}

void TileMap::drawTilePro(Vector2 tile, Rectangle dest, Color color, bool flipedX) {
    auto rect = getRectForTile(tile);
    
    if (flipedX) {
        rect.width *= -1;
    }

    DrawTexturePro(m_map, rect, dest, {0, 0}, 0, color);
}

Texture2D TileMap::loadTile(Vector2 pos) {
    Image mapimg = LoadImageFromTexture(m_map);
    
    ImageCrop(&mapimg, getRectForTile(pos));

    Texture2D tile = LoadTextureFromImage(mapimg);
    UnloadImage(mapimg);

    return tile;
}