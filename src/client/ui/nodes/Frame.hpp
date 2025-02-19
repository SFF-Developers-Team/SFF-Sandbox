#pragma once
#include <ui/nodes/Node.hpp>

class Frame : public Node {
protected:
    float m_border;

public:
    Frame();

    void setBorderWidth(float width) { m_border = width; }
    float getBorderWidth() { return m_border; }

    void draw();
};