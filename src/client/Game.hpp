#pragma once
#include <memory>
#include <string>
#include <vector>

class Scene;
class World;
class Player;
class TileMap;

class Game {
private:
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
    void pushScene(std::shared_ptr<Scene> scene);

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
