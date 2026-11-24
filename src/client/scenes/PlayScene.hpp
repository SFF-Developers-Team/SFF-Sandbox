#pragma once
#include <ui/basic/Scene.hpp>
#include <memory>

class ClientWorld;
class LocalPlayer;
class Timer;
class Node;
class List;

class PlayScene : public Scene {
private:
    World& m_world;
    std::shared_ptr<LocalPlayer> m_player;
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<List> m_playersList;
    
    bool m_online;
    bool m_paused;
    bool m_inventoryEnabled;

    std::vector<std::string> nickList;
    std::vector<std::shared_ptr<Node>> m_pauseNodes;
public:
    PlayScene(World& world, bool isOnline = false);
    ~PlayScene();

    void draw() override;
    void update() override;

    void drawWorld();
    void drawTile(raylib::Rectangle dest, TilePosition position);

    void setPaused(bool paused);
    void setInventoryOpened(bool isOpen);

    void keyBackClicked();

    void onPop() override;
};