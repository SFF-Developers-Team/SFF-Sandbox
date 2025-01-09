#pragma once
#include <Types.hpp>
#include <ui/UiManager.hpp>

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

    virtual void update() {}
    virtual void draw() {}
};