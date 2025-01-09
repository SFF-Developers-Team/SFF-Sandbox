#pragma once
#include <entity/Player.hpp>
#include <world/World.hpp>
#include <Multiplayer.hpp>
#include <RenderManager.hpp>
#include <TileMap.hpp>
#include <Timer.hpp>
#include <memory>
#include <raylib.h>

class Game {
private:
    int const m_screenWidth = 1280;
    int const m_screenHeight = 720;
    uint8_t m_renderDistance = 3;
    bool m_isMultiplayer = false;
    std::string m_username;

    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;
    std::shared_ptr<TileMap> m_blocksMap;

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
    void setRayGuiStyle();

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
