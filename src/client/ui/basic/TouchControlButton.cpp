#include <ui/basic/TouchControlButton.hpp>
#include <fix_win32_compatibility.h>
#include <raylib.h>

TouchControlButton::TouchControlButton(uint16_t tileIndex, bool hold, MiniFunction<void()> const& callback) 
    : TileNode("gui.png", tileIndex), m_hold(hold), m_callback(callback) {}

void TouchControlButton::update() {
    auto check = (m_hold) ? GESTURE_HOLD : GESTURE_TAP;

    if(isMouseHover() && IsGestureDetected(check)) {
        m_callback();
    }
}