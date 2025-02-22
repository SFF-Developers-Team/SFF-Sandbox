#pragma once
#include <ui/nodes/Container.hpp>

class ScrollableContainer : public Container {
protected:
    float calculateTotalHeight();

public:
    ScrollableContainer();

    void draw();
    void update();

    void resetScroll() { m_offset.y = 0.f; }
};