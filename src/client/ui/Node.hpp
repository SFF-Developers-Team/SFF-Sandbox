#pragma once
#include <Types.hpp>
#include <Utils.hpp>

class Node {
protected:
    Rectf m_bounds;

public:
    Node();

    void update();
    void draw();
};