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
class Container;

class Game {
private:
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

    void checkSceneFlags(std::shared_ptr<Container> root);

    void setUsername(std::string const& username) { m_username = username; }
    std::string const& getUsername() { return m_username; }

    std::shared_ptr<World> getWorld() { return m_world; }
    std::shared_ptr<Player> getPlayer() { return m_player; }
    
    Vec2i const getLastWindowSize() { return m_lastWindowSize; }
    
    int getGuiScale() { return m_guiScale; }
    void setGuiScale(int scale) { m_guiScale = scale; }
};
