#pragma once

#include <Scene.hpp>
#include <Game.hpp>
#include <Debug.hpp>
#include <Timer.hpp>
#include <TileMap.hpp>
#include <WorldGenNormal.hpp>
#include <RenderManager.hpp>
#include <string>
#include <cmath>
#include <vector>
#include <GitHash.hpp>

class PlayScene : public Scene {
private:

public:
    PlayScene();
    
    void draw() override;
    void update() override;
};

class MainScene : public Scene {
private:
public:
    void LoadTextures();
    void draw() override;
};

class MultiplayerScene : public Scene {
private:
    char* m_nickname;
    char* m_ip;

    bool m_isNick = 0;
    bool m_isIP = 0;

    Rectangle m_nickBox = {(float)(GetScreenWidth() - 200) / 2, 300, 200, 40};
    Rectangle m_ipBox = {(float)(GetScreenWidth() - 200) / 2, 360, 200, 40};

    MainScene m_menu;
public:
    MultiplayerScene();
    ~MultiplayerScene();
    void draw() override;
    void update() override;
};

class CreditsScene : public Scene {
private:

    struct Developer {
        Texture2D texture;
        std::string name;
        std::string role;
    };

    std::vector<Developer> devs = {
        // {m_dogotrigger, "dogotrigger", "programmer"},
        // {m_kolyah35, "Kolyah35", "programmer"},
        // {m_invisedivine, "InviseDivine", "programmer"},
        // {m_del, "del", "artist"},
        // {m_e2e4, "e2e4", "artist"},
    };

    const int yoffset = 30;
    const Vector2 picsize = {128, 128};
    const int textoffset = 10;
    const int textheight = 25;
    const int devsoffset = 30;

    float width = picsize.x * devs.size() + devsoffset * devs.size();
    float height = picsize.y + textheight * 2 + textoffset * 2; 
    Rectangle container = {(float)(GetScreenWidth() - 990) / 2, 305, 990, 390};

    Vector2 offset = {container.width / 2 - width / 2, container.height / 2 - height / 2 - yoffset + 305};
    Vector2 offsetTest = {container.width / 2 - width / 2, container.height / 2 - height / 2 - yoffset + 305};


public:

    void LoadTextures();
    void draw() override;
    void update() override;
};