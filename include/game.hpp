#pragma once
#include <raylib.h>
#include "World.hpp"
#include "Player.hpp"

class Game {
private:
    const int m_screenWidth = 1280;
    const int m_screenHeight = 720;
    Player* m_player;

public:
    World* m_world;

    int m_renderDistance = 4;
    bool m_debug = false;

    void init();
    void render();
    void update();
};