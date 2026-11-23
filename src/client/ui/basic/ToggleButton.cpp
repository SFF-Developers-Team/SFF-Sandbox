#include <raylib-cpp.hpp>
#include <ui/basic/ToggleButton.hpp>
#include <format>
#include <managers/RenderManager.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/StyleManager.hpp>
#include <Logger.hpp>

ToggleButton::ToggleButton(std::string const& text, MiniFunction<void(ToggleButton*, bool)> const& callback) : 
    Frame(), m_flag(false), m_callback(callback), m_text(text) {
    m_fontSize = ResourceManager::Get().GetFontBaseSize("boldfont");
}

void ToggleButton::draw() {
    Frame::draw();

    auto text = m_text + (m_flag ? ": ON" : ": OFF");
    auto& font = ResourceManager::Get().GetFont("boldfont");
    auto size = font.MeasureText(text, m_fontSize, 1.f);
    font.DrawText(text, {(getWidth() - size.x) / 2.f, (getHeight() - size.y) / 2.f}, m_fontSize, 1.f, raylib::Color::White());
}

void ToggleButton::update() {
    auto sm = StyleManager::get();
    auto mouse = raylib::Mouse::GetPosition();
    auto rect = getWorldBounds();
    m_color = sm->getValue<raylib::Color>(DEFAULT_UI_COLOR);

    if(rect.CheckCollision(mouse)) {
        m_color = m_color.Brightness(-0.3f * (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) + 1));
    }

    if(rect.CheckCollision(mouse) && raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
        callback();
    }
}

void ToggleButton::callback() {
    m_flag = !m_flag;

    if (m_callback) {
        m_callback(this, m_flag);
    }
}
