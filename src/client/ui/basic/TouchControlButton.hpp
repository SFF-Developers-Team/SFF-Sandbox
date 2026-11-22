#pragma once
#include <ui/basic/TileNode.hpp>
#include <MiniFunction.hpp>

class TouchControlButton : public TileNode {
    MiniFunction<void()> m_callback;
    bool m_hold;

public:
    TouchControlButton(uint16_t tileIndex, bool hold, MiniFunction<void()> const& callback);

    void update();
};