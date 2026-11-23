#pragma once
#include <raylib-cpp.hpp>
#include <ui/basic/Frame.hpp>
#include <MiniFunction.hpp>
#include <type_traits>
#include <managers/RenderManager.hpp>
#include <algorithm>
#include <format>
#include <managers/ResourceManager.hpp>

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

        auto& rm = ResourceManager::Get();
    
        auto range = static_cast<float>(m_max - m_min);
        auto percent = range != 0.f ? static_cast<float>(m_value - m_min) / range : 0.f;
        auto thumb = raylib::Rectangle(m_border + (getWidth() - m_border * 5) * percent, m_border, m_border * 3, getHeight() - m_border * 2);
    
        RenderManager::drawFrame(thumb, m_color, m_border);

        auto& font = rm.GetFont("font");
        std::string label;
        if constexpr (std::is_floating_point_v<T>) {
            label = std::format("{}{:.2f}", m_text, m_value);
        } else {
            label = std::format("{}{}", m_text, m_value);
        }

        font.DrawText(label, getSize() / 2, font.MeasureText(label, font.baseSize, 1.f) / 2, 0.f, font.baseSize, 1.f);
    }
        
    void update() {
        Frame::update();
    
        auto bounds = getWorldBounds();
        bounds.x += m_border;
        bounds.width -= m_border * 2;
    
        auto mouse = raylib::Mouse::GetPosition();
    
        m_prevValue = m_value;
    
        if(raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON) && bounds.CheckCollision(mouse)) {
            m_selected = true;
        }
    
        if(raylib::Mouse::IsButtonReleased(MOUSE_LEFT_BUTTON)) {
            m_selected = false;
        }
    
        if(m_selected || (isMouseHover() && raylib::Mouse::GetWheelMove() != 0.f)) {
            if (m_selected) {
                m_value = (mouse.x - bounds.x) / bounds.width * (m_max - m_min) + m_min;
            }

            m_value += raylib::Mouse::GetWheelMove() * (m_max - m_min) / 10;
            m_value = std::clamp(m_value, m_min, m_max);

            if(m_prevValue != m_value) callback();
        }
    }

    T getValue() { return m_value; }
    void setValue(T value) { m_value = value; }
};