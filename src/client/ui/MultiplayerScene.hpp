#pragma once
#include <ui/Scene.hpp>

class MultiplayerScene : public Scene {
private:
    bool m_isNick = 0;
    bool m_isIP = 0;
public:
    MultiplayerScene();
    ~MultiplayerScene();
    
    void draw() override;
};