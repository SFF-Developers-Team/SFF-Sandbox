#pragma once

#include <raylib.h>
#include "raygui.h"
#include <World.hpp>
#include <Menu.hpp>
#include <Player.hpp>
#include <TileMap.hpp>
#include <Timer.hpp>
#include <Scene.hpp>
#include <ParticleManager.hpp>
#include <RenderManager.hpp>
#include <Multiplayer.hpp>
#include <memory>

class Game {
private:
    const int m_screenWidth = 1280;
    const int m_screenHeight = 720;
    uint8_t m_renderDistance = 3;
    bool m_isMultiplayer = false;
    bool m_inPlayScene = false;
    std::string m_username;
        
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;
    std::shared_ptr<ParticleManager> m_particleManager;
    std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<TileMap> m_blocksMap;
    std::shared_ptr<Timer> m_timer;
    
public:
    static Game* get() {
        static Game* game = new Game();
        return game;
    }

    void init(std::vector<std::string>& args);
    void render();
    void update();

    void renderMenu();
    void updateMenu();

    void pushScene(std::shared_ptr<Scene> scene);
    void drawCrosshair(Vector2 pos);
    void setInPlayScene(bool flag) { m_inPlayScene = flag; }
    void setRayGuiStyle();
    
    auto getScreenWidth() { return m_screenWidth; }
    auto getScreenHeight() { return m_screenHeight; }
    auto getRenderDistance() { return m_renderDistance; }
    auto getWorld() { return m_world; }
    auto getBlocksTileMap() { return m_blocksMap; }
    auto getParticleManager() { return m_particleManager; }
    auto getUsername() { return m_username; }
    auto getPlayer() { return m_player; }
    auto isMultiplayer() { return m_isMultiplayer; }
    auto getTimer() { return m_timer; }
};
