#pragma once
#include <ui/nodes/Frame.hpp>
#include <MiniFunction.hpp>
#include <type_traits>
#include <RenderManager.hpp>
#include <raylib.h>
#include <algorithm>
#include <format>

template <typename T>
class Slider : public Frame {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Slider");

protected:
    MiniFunction<void(Slider*, T)> m_callback;
    std::string m_text;
    T m_min;
    T m_max;
    T m_prevValue;
    T m_value;
    bool m_selected;

public:
    Slider(std::string const& text, T min, T max, MiniFunction<void(Slider*, T)> const& callback)
        : Frame(), m_text(text), m_min(min), m_max(max), m_value(min), m_selected(false), m_callback(callback) {}

    void draw() {
        Frame::draw();
    
        auto percent = static_cast<float>(m_value - m_min) / static_cast<float>(m_max - m_min);
        auto thumb = Rectf {m_border + (m_bounds.width - m_border * 5) * percent, m_border, m_border * 3, m_bounds.height - m_border * 2};
    
        auto rm = RenderManager::get();
        rm->drawText("font", std::format((std::is_floating_point_v<T> ? "{}{:.2f}" : "{}{}"), m_text, m_value), {m_bounds.width / 2, m_bounds.height / 2}, COL_WHITE, 40.f, {0.5f, 0.5f});
        rm->drawRect(thumb, m_color);
        rm->drawRectLines(thumb, m_color - Col4u {0x7F, 0x7F, 0x7F, 0x7F}, m_border);
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
    
        if(m_selected) {
            m_value = (mouse.x - bounds.x) / bounds.width * (m_max - m_min) + m_min;
            m_value = std::clamp(m_value, m_min, m_max);
    
            if(m_prevValue != m_value) {
                m_callback(this, m_value);
            }
        }
    }

    T getValue() { return m_value; }
    void setValue(T value) { m_value = value; }
};