#pragma once
#include <ui/basic/Container.hpp>

class ScrollableContainer : public Container {
public:
    ScrollableContainer();

    void draw();
    void update();

    void resetScroll() { m_offset.y = 0.f; }
    Vec2f getTotalSize();
};