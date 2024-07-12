#pragma once
#include <raylib.h>
#include <World.hpp>
#include <Player.hpp>
#include <TileMap.hpp>

class Game {
private:
    const int m_screenWidth = 1280;
    const int m_screenHeight = 720;
    int m_renderDistance = 3;
    World* m_world;

public:
    TileMap* m_blocksMap;
    
    static Game* get() {
        static Game* game = new Game();
        return game;
    }

    ~Game();

    void init();
    void render();
    void update();

    int getScreenWidth() {
        return m_screenWidth;
    }

    int getScreenHeight() {
        return m_screenHeight;
    }

    int getRenderDistance() {
        return m_renderDistance;
    }

    World* getWorld() {
        return m_world;
    }

    TileMap* getBlocksTileMap() {
        return m_blocksMap;
    }
};
