#include <TileMap.hpp>

TileMap::TileMap(std::filesystem::path const& path, Vector2 v) {
    this->m_map = LoadTexture(path.string().c_str());
    this->m_tileSize = v;
}

TileMap::~TileMap() {
    UnloadTexture(this->m_map);

    for(auto& [k, v] : m_cachedTextures) {
        UnloadTexture(m_cachedTextures[k]);
    }
}

Vector2 TileMap::getPositionByIndex(uint16_t index) {
    return Vector2 {floorf(index % (int)(m_map.width / m_tileSize.x)), floorf(index / (m_map.height / m_tileSize.y))};
}

Rectangle TileMap::getRectForTile(Vector2 position) {
    Rectangle r;

    r.width = floor(m_tileSize.x);
    r.height = floor(m_tileSize.y);
    r.x = ceil(position.x * floor(m_tileSize.x));
    r.y = ceil(position.y * floor(m_tileSize.y));

    return r;
}

Rectangle TileMap::getRectForTile(uint16_t index) {
    return getRectForTile(getPositionByIndex(index));
}

void TileMap::drawTile(uint16_t index, Vector2 position, Color color, bool flipedX) {
    auto rect = getRectForTile(index);
    
    if (flipedX) {
        rect.width *= -1;
    }

    DrawTextureRec(m_map, rect, position, color);
}

void TileMap::drawTilePro(uint16_t index, Rectangle dest, Color color, bool flipedX) {
    auto rect = getRectForTile(index);
    
    if (flipedX) {
        rect.width *= -1;
    }

    DrawTexturePro(m_map, rect, dest, {0, 0}, 0, color);
}

Texture2D TileMap::getTextureOfTileCached(uint16_t index) {
    if(m_cachedTextures.count(index)) {
        return m_cachedTextures[index];
    }
    
    Image mapimg = LoadImageFromTexture(m_map);
    ImageCrop(&mapimg, getRectForTile(index));

    Texture2D tile = LoadTextureFromImage(mapimg);
    UnloadImage(mapimg);

    m_cachedTextures[index] = tile;

    return tile;
}