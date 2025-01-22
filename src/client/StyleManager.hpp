#pragma once
#include <unordered_map>
#include <variant>
#include <Types.hpp>

enum StyleKey : uint8_t {
    FIRST_COLOR_NORMAL,
    SECOND_COLOR_NORMAL,
    TEXT_COLOR_NORMAL,

    FIRST_COLOR_FOCUS,
    SECOND_COLOR_FOCUS,
    TEXT_COLOR_FOCUS,

    FIRST_COLOR_PRESS,
    SECOND_COLOR_PRESS,
    TEXT_COLOR_PRESS,

    TEXT_SIZE,
    BORDER_WIDTH
};

using StyleValue = std::variant<Col4u, float>;

class StyleManager {
private:
    std::unordered_map<StyleKey, StyleValue> m_style;

public:
    static auto get() {
        static auto sm = new StyleManager();
        return sm;
    }

    StyleManager();

    template<typename T>
    T getValue(StyleKey key) {
        return std::get<T>(m_style[key]);
    }

    void setValue(StyleKey key, StyleValue value);

    bool hasValue(StyleKey key);
};