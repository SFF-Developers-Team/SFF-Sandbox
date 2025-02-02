#pragma once
#include <ui/nodes/Node.hpp>

class Frame : public Node {
protected:
    float m_border;

public:
    Frame();

    void setBorderWidth(float width);
    float getBorderWidth();

    void draw();
};