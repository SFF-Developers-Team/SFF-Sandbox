#pragma once
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <Types.hpp>

class Scene;
class World;
class Player;
class TileMap;

class Game {
private:
    uint8_t m_renderDistance = 3;
    std::string m_username;

    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;

    std::deque<std::shared_ptr<Scene>> m_sceneHistory;

    Vec2i m_lastWindowSize;
    int m_guiScale = 1;
    
public:
    static Game* get() {
        static Game* game = new Game();
        return game;
    }

    void init(std::vector<std::string>& args);
    void render();
    void update();
    void destroy();
    
    void clearSceneHistory();
    void pushScene(std::shared_ptr<Scene> scene);
    void popScene();

    void setUsername(std::string const& username) {
        m_username = username;
    }

    auto getUsername() {
        return m_username;
    }

    auto getRenderDistance() {
        return m_renderDistance;
    }
    
    auto getWorld() {
        return m_world;
    }

    auto getPlayer() {
        return m_player;
    }
    
    Vec2i const getLastWindowSize() { return m_lastWindowSize; }

    int getGuiScale() { return m_guiScale; }
    void setGuiScale(int scale) { m_guiScale = scale; }
};
