#include <TileMap.hpp>

TileMap::TileMap(std::filesystem::path const& path, Vec2i v) {
    this->m_map = LoadTexture(path.string().c_str());
    this->m_tileSize = v;
}

TileMap::~TileMap() {
    UnloadTexture(this->m_map);

    for (auto& [k, v] : m_cachedTextures) {
        UnloadTexture(m_cachedTextures[k]);
    }
}

Vec2i TileMap::getPositionByIndex(uint16_t index) {
    return {
        index % (m_map.width / m_tileSize.x), 
        index / (m_map.height / m_tileSize.y)
    };
}

Rectangle TileMap::getRectForTile(Vec2i position) {
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

Texture2D& TileMap::getTextureOfTileCached(uint16_t index) {
    if (m_cachedTextures.contains(index)) {
        return m_cachedTextures[index];
    }

    Image mapimg = LoadImageFromTexture(m_map);
    ImageCrop(&mapimg, getRectForTile(index));

    Texture2D tile = LoadTextureFromImage(mapimg);
    UnloadImage(mapimg);

    m_cachedTextures[index] = tile;

    return m_cachedTextures[index];
}

Texture2D& TileMap::getMap() {
    return m_map;
}