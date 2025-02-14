#include <StyleManager.hpp>

StyleManager::StyleManager() {
    m_style = {
        {DEFAULT_UI_COLOR, Col4u {0x1A, 0x1C, 0x47, 0xFF}},
        {DEFAULT_ELEMENT_SIZE, Vec2f {200.f, 20.f}},
        {DEFAULT_BORDER_WIDTH, 2.5f}
    };
}

void StyleManager::setValue(StyleKey key, StyleValue value) {
    m_style[key] = value;
}

bool StyleManager::hasValue(StyleKey key) {
    return m_style.contains(key);
}