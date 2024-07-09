#pragma once
#include <raylib.h>
#include "World.hpp"

class Game {
private:
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const int cameraSpeed = 6;

    Camera2D camera;

public:
    World world;

    void init();
    void render();
    void updateControls();
};