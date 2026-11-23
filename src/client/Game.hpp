#pragma once
#include <Types.hpp>
#include <raylib-cpp.hpp>
#include <optional>
#include <memory>
#include <string>
#include <vector>
#include <deque>

class Scene;
class ClientWorld;
class LocalPlayer;
class Container;

class Game {
private:
    raylib::Window m_window;
    std::optional<raylib::AudioDevice> m_audioDevice;
    std::string m_username;

    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<ClientWorld> m_world;
    std::shared_ptr<LocalPlayer> m_player;

    std::deque<std::shared_ptr<Scene>> m_sceneHistory;

    raylib::Vector2 m_lastWindowSize;
    int m_guiScale;
    bool m_close;

    ControlType m_controlType;
    
public:
    static Game* get() {
        static Game* game = new Game();
        return game;
    }

    void init(std::vector<std::string>& args);
    void render();
    void update();
    void destroy();
    void close() { m_close = true; }
    raylib::Window& getWindow() { return m_window; }
    raylib::AudioDevice& getAudioDevice() { return *m_audioDevice; }
    
    auto getScene() { return m_scene; }
    void clearSceneHistory() { m_sceneHistory.clear(); }
    void setScene(std::shared_ptr<Scene> scene) { m_scene = scene; }
    void pushScene(std::shared_ptr<Scene> scene);
    void popScene();

    void checkSceneFlags(std::shared_ptr<Container> root);

    void setUsername(std::string const& username) { m_username = username; }
    std::string const& getUsername() { return m_username; }

    std::shared_ptr<ClientWorld> getWorld() { return m_world; }
    void setWorld(std::shared_ptr<ClientWorld> world) { m_world = world; };

    std::shared_ptr<LocalPlayer> getPlayer() { return m_player; }
    void setPlayer(std::shared_ptr<LocalPlayer> player) { m_player = player; };
    
    raylib::Vector2 const getLastWindowSize() { return m_lastWindowSize; }
    
    int getGuiScale() { return m_guiScale; }
    void setGuiScale(int scale) { m_guiScale = scale; }
    static int getMaximumGuiScale();
    void updateGuiScale();

    ControlType getControlType() { return m_controlType; }
};
