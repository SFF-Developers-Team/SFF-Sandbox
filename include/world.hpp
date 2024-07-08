enum Blocks {
    AIR,
    GRASS,
    DIRT,
    STONE
};



#pragma once
#include <raylib.h>
#include <game.hpp>

class World {
private:
    int worldArr[256][64];
public:

    void Generate();
    void WorldDraw();
    // void Rectangle
};  
