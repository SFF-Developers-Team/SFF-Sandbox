#pragma once
#include <ui/nodes/Scene.hpp>
#include <memory>

class World;
class Player;
class Timer;
class Node;

class PlayScene : public Scene {
private:
    std::shared_ptr<World> m_world;
    std::shared_ptr<Player> m_player;
    std::shared_ptr<Timer> m_timer;

    bool m_online;
    bool m_paused;
    bool m_inventoryEnabled;

    std::vector<std::string> nickList;
    std::vector<std::shared_ptr<Node>> m_pauseNodes;
public:
    PlayScene(bool isOnline = false);
    ~PlayScene();

    void draw() override;
    void update() override;

    void setPaused(bool paused);
    void setInventoryOpened(bool isOpen);

    void keyBackClicked();
};