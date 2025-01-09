#pragma once
#include <ui/Scene.hpp>

class World;
class Player;
class Timer;

class PlayScene : public Scene {
private:
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;
    std::shared_ptr<Timer> m_timer;

public:
    PlayScene(std::shared_ptr<World> world, std::shared_ptr<Player> player);
    
    void draw() override;
    void update() override;
};