#include <TileMap.hpp>

using namespace fightable;

TileMap::TileMap(std::string path, Vector2 v) {
    this->_map = LoadTexture(path.c_str());
    this->_tileSize = v;
}

TileMap::~TileMap() {
    UnloadTexture(this->_map);

    clearDrawEffects();
}

Rectangle TileMap::getRectForTile(Vector2 position) {
    Rectangle r;

    r.width = _tileSize.x;
    r.height = _tileSize.y;
    r.x = position.x * _tileSize.x;
    r.y = position.y * _tileSize.y;

    return r;
}

void TileMap::drawTile(Vector2 tile, Vector2 position, bool flipedX) {
    Color tint = WHITE;

    for (auto effect : _effects) {
        auto _1 = dynamic_cast<TileMapEffectColor *>(effect);

        if (_1) {
            tint = _1->_tint;
        }
    }

    auto rect = getRectForTile(tile);
    
    if (flipedX) {
        rect.width *= -1;
    }

    DrawTextureRec(_map, rect, position, tint);
}

void TileMap::applyDrawEffect(TileMapEffect &effect) {
    auto _1 = dynamic_cast<TileMapEffectColor *>(&effect);

    if (_1) {
        TileMapEffectColor *_ = new TileMapEffectColor(*_1);
        _effects.push_back(_);

        return;
    }

    TileMapEffect *_ = new TileMapEffect(effect);
    _effects.push_back(_);
}
void TileMap::clearDrawEffects() {
    for (auto effect : _effects) {
        delete effect;
    }

    _effects.clear();
}

Texture2D TileMap::loadTile(Vector2 tile) {
    Image mapimg = LoadImageFromTexture(_map);
    
    ImageCrop(&mapimg, getRectForTile(tile));

    Texture2D _tile = LoadTextureFromImage(mapimg);
    UnloadImage(mapimg);

    return _tile;
}