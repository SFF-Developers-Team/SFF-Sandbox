#pragma once
#include <raylib.h>
#include <World.hpp>
#include <Player.hpp>
#include <TileMap.hpp>
#include <ParticleManager.hpp>

class Game {
private:
    const int m_screenWidth = 1280;
    const int m_screenHeight = 720;
    uint8_t m_renderDistance = 3;
    
    World* m_world;
    ParticleManager* m_particleManager;

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

    void drawCrosshair(Vector2 pos);

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

    ParticleManager* getParticleManager() {
        return m_particleManager;
    }
};
