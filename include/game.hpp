#pragma once
#include <raylib.h>
#include "World.hpp"

class Game {
private:
    const int screenWidth = 1280;
    const int screenHeight = 720;

public:
    static inline Texture2D Grass;
    static inline Texture2D Dirt;
    static inline Texture2D Stone;

    World world;

    void init();
    void render();
};