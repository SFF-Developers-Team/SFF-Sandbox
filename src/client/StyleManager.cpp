#include <StyleManager.hpp>

StyleManager::StyleManager() {
    m_style = {
        {SECOND_COLOR_NORMAL, Col4u {0x29, 0x2B, 0x56, 0xFF}},
        {FIRST_COLOR_NORMAL, Col4u {0x1A, 0x1C, 0x47, 0xFF}},
        {TEXT_COLOR_NORMAL, Col4u {0xFF, 0xFF, 0xFF, 0xFF}},

        {SECOND_COLOR_FOCUS, Col4u {0x38, 0x3A, 0x65, 0xff}},
        {FIRST_COLOR_FOCUS, Col4u {0x29, 0x2B, 0x56, 0xff}},
        {TEXT_COLOR_FOCUS, Col4u {0xD8, 0xD8, 0xD8, 0xFF}},
        
        {SECOND_COLOR_PRESS, Col4u {0x46, 0x47, 0x70, 0xff}},
        {FIRST_COLOR_PRESS, Col4u {0x29, 0x2B, 0x56, 0xff}},
        {TEXT_COLOR_PRESS, Col4u {0xD8, 0xD8, 0xD8, 0xFF}},

        {TEXT_SIZE, 20.f},
        {BORDER_WIDTH, 5.f}
    };
}

void StyleManager::setValue(StyleKey key, StyleValue value) {
    m_style[key] = value;
}

bool StyleManager::hasValue(StyleKey key) {
    return m_style.contains(key);
}