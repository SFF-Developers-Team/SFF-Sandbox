#pragma once
#include <ui/basic/Frame.hpp>
#include <MiniFunction.hpp>
#include <type_traits>
#include <managers/RenderManager.hpp>
#include <raylib.h>
#include <algorithm>
#include <format>
#include <managers/TextureManager.hpp>

template <typename T>
class Slider : public Frame {
    // static_assert(std::is_arithmetic_v<T>, "Unsupported type for Slider");

protected:
    MiniFunction<void(Slider*, T)> m_callback;
    std::string m_text;
    T m_min;
    T m_max;
    T m_prevValue;
    T m_value;
    bool m_selected;

    virtual void callback() { m_callback(this, m_value); }

public:
    Slider(std::string const& text, T min, T max, MiniFunction<void(Slider*, T)> const& callback)
        : Frame(), m_text(text), m_min(min), m_max(max), m_value(min), m_selected(false), m_callback(callback) {}

    void draw() {
        Frame::draw();

        auto fontSize = TextureManager::get()->getFontBaseSize("font");
    
        auto percent = static_cast<float>(m_value - m_min) / static_cast<float>(m_max - m_min);
        auto thumb = Rectf {m_border + (getWidth() - m_border * 5) * percent, m_border, m_border * 3, getHeight() - m_border * 2};
    
        RenderManager::drawText("font", std::format((std::is_floating_point_v<T> ? "{}{:.2f}" : "{}{}"), m_text, m_value), {getWidth() / 2, getHeight() / 2}, COL_WHITE, fontSize, {0.5f, 0.5f});
        RenderManager::drawFrame(thumb, m_color, m_border);
    }
        
    void update() {
        Frame::update();
    
        auto bounds = getWorldBounds();
        bounds.x += m_border;
        bounds.width -= m_border * 2;
    
        auto mouse = GetMousePosition();
    
        m_prevValue = m_value;
    
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && bounds.contains({mouse.x, mouse.y})) {
            m_selected = true;
        }
    
        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            m_selected = false;
        }
    
        if(m_selected || (isMouseHover() && GetMouseWheelMove() != 0.f)) {
            if (m_selected) {
                m_value = (mouse.x - bounds.x) / bounds.width * (m_max - m_min) + m_min;
            }

            m_value += GetMouseWheelMove() * (m_max - m_min) / 10;
            m_value = std::clamp(m_value, m_min, m_max);

            if(m_prevValue != m_value) callback();
        }
    }

    T getValue() { return m_value; }
    void setValue(T value) { m_value = value; }
};