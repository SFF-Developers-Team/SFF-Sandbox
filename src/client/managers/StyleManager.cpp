#include <managers/StyleManager.hpp>

StyleManager::StyleManager() {
    m_style = {
        {DEFAULT_UI_COLOR, raylib::Color{0, 50, 100, 255}},
        {DEFAULT_ELEMENT_SIZE, raylib::Vector2 {200.f, 20.f}},
        {DEFAULT_BORDER_WIDTH, 2.5f}
    };
}

void StyleManager::setValue(StyleKey key, StyleValue value) {
    m_style[key] = value;
}

bool StyleManager::hasValue(StyleKey key) {
    return m_style.contains(key);
}