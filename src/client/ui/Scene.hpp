#pragma once

#include <raylib.h>
class Scene {
protected:
    Color m_bgColor = SKYBLUE;
public:
    Color getColor() { return m_bgColor; }
    virtual void setColor(Color color) { m_bgColor = color; }
    virtual void update();
    virtual void draw();
};