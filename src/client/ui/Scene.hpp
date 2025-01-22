#pragma once
#include <ui/UiManager.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <raylib.h>

class Scene : protected UiManager {
protected:
    Col4u m_bgColor = COL_WHITE;
    bool m_shouldExit = false;

public:
    Scene();

    Col4u getColor();
    void setColor(Col4u color);
    void exit();

    virtual void update();
    virtual void draw();

    virtual void onPush() {}
    virtual void onPop() {};
};