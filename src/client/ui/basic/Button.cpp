#include <raylib-cpp.hpp>
#include <ui/basic/Button.hpp>
#include <ui/basic/Text.hpp>
#include <managers/StyleManager.hpp>
#include <managers/RenderManager.hpp>
#include <managers/ResourceManager.hpp>

Button::Button(std::string const& text, MiniFunction<void(Button*)> const& callback) : Frame(), m_text(text), m_callback(callback), m_fontSize(0.f) {}

void Button::update() {
    auto sm = StyleManager::get();
    m_color = sm->getValue<raylib::Color>(DEFAULT_UI_COLOR);
    
    if(isMouseHover()) {
        m_color = m_color.Brightness(-0.3f * (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) + 1));
    
        if(raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
            m_callback(this);
        }
    }
}

void Button::draw() {
    Frame::draw();
    auto& font = ResourceManager::Get().GetFont("boldfont");
    float fontSize = m_fontSize > 0.f ? m_fontSize : static_cast<float>(font.baseSize);
    auto textSize = font.MeasureText(m_text, fontSize, 1.f);
    font.DrawText(m_text, (getSize() - textSize) / 2, fontSize, 1.f, raylib::Color::White());
}

void Button::setText(std::string const& text) {
    m_text = text;
}