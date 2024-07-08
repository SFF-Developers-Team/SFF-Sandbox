#pragma once

#include <raylib.h>
#include <game.hpp>
#include <world.hpp>
class Game {
private:

    const int screenWidth = 1280;
    const int screenHeight = 720;

public:
    static inline Texture2D Grass;
    static inline Texture2D Dirt;
    static inline Texture2D Stone;
    void init();
    void render();


};