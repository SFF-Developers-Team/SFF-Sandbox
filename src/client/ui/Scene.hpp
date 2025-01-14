#pragma once
#include <ui/UiManager.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <raylib.h>

class Scene : protected UiManager {
protected:
    Col4u m_bgColor = COL_WHITE;

public:
    Scene() {};
    Col4u getColor() {
        return m_bgColor;
    }

    virtual void setColor(Col4u color) {
        m_bgColor = color;
    }

    virtual void update() {
        if(IsKeyPressed(KEY_ESCAPE)) {
            Game::get()->popScene();
        }
    }
    virtual void draw() { m_nodeCount = 0; }
};