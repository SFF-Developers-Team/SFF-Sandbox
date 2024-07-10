#pragma once

#include <raylib.h>
#include <vector>
#include <TileMapEffects.hpp>
#include <string>

namespace fightable {
    class TileMap {
    protected:
        Texture2D _map;
        Vector2 _tileSize = {0, 0};

        std::vector<TileMapEffect *> _effects;
    public:
        TileMap(std::string path, Vector2 tileSize);
        ~TileMap();

        void applyDrawEffect(TileMapEffect &effect);
        void clearDrawEffects();

        Rectangle getRectForTile(Vector2 position);
        void drawTile(Vector2 tile, Vector2 position, bool flipedX = false);

        Texture2D loadTile(Vector2 tile);
    };
}