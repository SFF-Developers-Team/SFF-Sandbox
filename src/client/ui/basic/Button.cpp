#include <ui/basic/Button.hpp>
#include <ui/basic/Text.hpp>
#include <raylib.h>
#include <StyleManager.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>

Button::Button(std::string const& text, MiniFunction<void(Button*)> const& callback) : Frame(), m_text(text), m_callback(callback), m_fontSize(0.f) {}

void Button::update() {
    auto sm = StyleManager::get();
    m_color = sm->getValue<Col4u>(DEFAULT_UI_COLOR);
    
    if(isMouseHover()) {
        m_color.brightness(-0.3f * (IsMouseButtonDown(MOUSE_BUTTON_LEFT) + 1));
    
        if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            m_callback(this);
        }
    }
}

void Button::draw() {
    Frame::draw();
    RenderManager::drawText("boldfont", m_text, {m_bounds.width / 2, m_bounds.height / 2}, COL_WHITE, m_fontSize, {0.5f, 0.5f});
}

void Button::setText(std::string const& text) {
    m_text = text;
}