#pragma once
#include <ui/basic/Tile.hpp>
#include <MiniFunction.hpp>

class TouchControlButton : public Tile {
    MiniFunction<void()> m_callback;
    bool m_hold;

public:
    TouchControlButton(uint16_t tileIndex, bool hold, MiniFunction<void()> const& callback);

    void update();
};