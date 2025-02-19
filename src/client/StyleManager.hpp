#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include <Types.hpp>

enum StyleKey : uint8_t {
    DEFAULT_UI_COLOR,
    DEFAULT_BORDER_WIDTH,
    DEFAULT_ELEMENT_SIZE
};

using StyleValue = std::variant<Col4u, Vec2f, float>;

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