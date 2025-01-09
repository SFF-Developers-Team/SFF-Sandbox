#pragma once
#include <Types.hpp>

class Scene {
protected:
    Col4u m_bgColor = COL_WHITE;

public:
    Col4u getColor() {
        return m_bgColor;
    }

    virtual void setColor(Col4u color) {
        m_bgColor = color;
    }

    virtual void update() = 0;
    virtual void draw() = 0;
};