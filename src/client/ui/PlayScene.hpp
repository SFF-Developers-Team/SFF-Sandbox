#pragma once
#include <ui/nodes/Scene.hpp>
#include <memory>

class World;
class Player;
class Timer;

class PlayScene : public Scene {
private:
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;
    std::shared_ptr<Timer> m_timer;

    bool m_online = false;

public:
    PlayScene(bool isOnline = false);
    ~PlayScene();

    void draw() override;
    void update() override;
};