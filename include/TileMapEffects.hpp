#pragma once

#include <raylib.h>

namespace fightable {
    class TileMap;

    class TileMapEffect {
    public:
        virtual ~TileMapEffect() = default;

        friend TileMap;
    };

    class TileMapEffectColor : public TileMapEffect {
    protected:
        Color _tint;
    public:
        TileMapEffectColor(Color col) { _tint = col;};

        friend TileMap;
    };
}        