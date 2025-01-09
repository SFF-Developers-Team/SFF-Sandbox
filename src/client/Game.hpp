#pragma once
#include <entity/Player.hpp>
#include <world/World.hpp>
#include <Multiplayer.hpp>
#include <RenderManager.hpp>
#include <SoundManager.hpp>
#include <TileMap.hpp>
#include <Timer.hpp>
#include <memory>
#include <raylib.h>

class Game {
private:
    int const static m_bgwidth = 11;
    int const static m_bgheight = 6;

    int const  m_screenWidth = 1280;
    int const  m_screenHeight = 720;

    uint8_t m_renderDistance = 3;
    bool m_isMultiplayer = false;
    std::string m_username;

    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;
    std::shared_ptr<TileMap> m_blocksMap;
    std::shared_ptr<Timer> m_timer;
    // 0 - GRASS, 1 - DIRT, 2 - STONE, 4 - AIR
    std::array<uint8_t, m_bgheight * m_bgwidth > m_bgArr = {
        0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 1, 0, 4, 4, 4, 4, 4, 4, 4, 4,
        1, 1, 1, 0, 0, 4, 4, 4, 4, 4, 4,
        1, 1, 1, 1, 1, 0, 0, 4, 4, 4, 4,
        1, 1, 1, 1, 1, 1, 1, 0, 0, 4, 4,
        2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0
    };
    RenderTexture2D m_bgTex;
    Camera2D m_cameraMenu;
public:
    static Game* get() {
        static Game* game = new Game();
        return game;
    }

    void init(std::vector<std::string>& args);

    void renderMenu();
    void updateMenu();

    void pushScene(std::shared_ptr<Scene> scene);
    void setRayGuiStyle();
    void setIsInPlayScene(bool is) { m_inPlayScene = is; }
    void menuPreRender();

    auto getScreenWidth() {
        return m_screenWidth;
    }
    auto getScreenHeight() {
        return m_screenHeight;
    }
    auto getRenderDistance() {
        return m_renderDistance;
    }
    auto getWorld() {
        return m_world;
    }
    auto getBlocksTileMap() {
        return m_blocksMap;
    }
    auto getUsername() {
        return m_username;
    }
    auto getPlayer() {
        return m_player;
    }
    auto isMultiplayer() {
        return m_isMultiplayer;
    }
};
