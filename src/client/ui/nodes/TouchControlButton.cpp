#include <ui/nodes/TouchControlButton.hpp>
#include <raylib.h>

TouchControlButton::TouchControlButton(uint16_t tileIndex, bool hold, MiniFunction<void()> const& callback) 
    : Tile("gui.png", tileIndex), m_hold(hold), m_callback(callback) {}

void TouchControlButton::update() {
    auto check = (m_hold) ? GESTURE_HOLD : GESTURE_TAP;

    if(isMouseHover() && IsGestureDetected(check)) {
        m_callback();
    }
}