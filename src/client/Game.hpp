#pragma once
#include <raylib.h>
#include <World.hpp>
#include <Player.hpp>
#include <TileMap.hpp>
#include <ParticleManager.hpp>
#include <RenderManager.hpp>
#include <Multiplayer.hpp>

class Game {
private:
    const int m_screenWidth = 1280;
    const int m_screenHeight = 720;
    uint8_t m_renderDistance = 3;
    bool m_multiplayer = false;
    std::string m_username;
    
    World* m_world;
    Player* m_player;
    ParticleManager* m_particleManager;
    RenderManager* m_renderManager;
    TileMap* m_blocksMap;
    Multiplayer* m_multiplayerManager;

public:
    static Game* get() {
        static Game* game = new Game();
        return game;
    }

    ~Game();

    void init(std::vector<std::string>& args);
    void render();
    void update();

    void drawCrosshair(Vector2 pos);

    auto getScreenWidth() { return m_screenWidth; }
    auto getScreenHeight() { return m_screenHeight; }
    auto getRenderDistance() { return m_renderDistance; }
    auto getWorld() { return m_world; }
    auto getBlocksTileMap() { return m_blocksMap; }
    auto getParticleManager() { return m_particleManager; }
    auto getUsername() { return m_username; }
    auto getPlayer() { return m_player; }
    auto getMultiplayer() { return m_multiplayerManager; }
    auto isMultiplayer() { return m_multiplayer; }
};
