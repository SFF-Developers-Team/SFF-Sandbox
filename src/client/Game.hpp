#pragma once
#include <raylib.h>
#include <World.hpp>
#include <Player.hpp>
#include <TileMap.hpp>
#include <Timer.hpp>
#include <ParticleManager.hpp>
#include <RenderManager.hpp>
#include <Multiplayer.hpp>
#include <ui/InitialMenu.hpp>
#include <ui/NodeRenderer.hpp>
#include <memory>

class Game {
private:
    const int m_screenWidth = 1280;
    const int m_screenHeight = 720;
    uint8_t m_renderDistance = 3;
    bool m_multiplayer = false;
    bool m_inPlayScene = false;
    std::string m_username;
    
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;
    std::shared_ptr<ParticleManager> m_particleManager;
    std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<TileMap> m_blocksMap;
    std::shared_ptr<Multiplayer> m_multiplayerManager;
    std::shared_ptr<Timer> m_timer;

    std::shared_ptr<sandbox_ui::InitialMenu> m_gameMenu;
    std::shared_ptr<sandbox_ui::NodeRenderer> m_uiRenderer;
public:
    static Game* get() {
        static Game* game = new Game();
        return game;
    }

    void init(std::vector<std::string>& args);
    void render();
    void update();

    void drawCrosshair(Vector2 pos);
    void setInPlayScene(bool flag) { m_inPlayScene = flag; }

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
