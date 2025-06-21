#include <ui/basic/ToggleButton.hpp>
#include <raylib.h>
#include <format>
#include <managers/RenderManager.hpp>
#include <managers/TextureManager.hpp>
#include <managers/StyleManager.hpp>
#include <Logger.hpp>

ToggleButton::ToggleButton(std::string const& text, MiniFunction<void(ToggleButton*, bool)> const& callback) : 
    Frame(), m_flag(false), m_callback(callback), m_text(text) {
    m_fontSize = TextureManager::get()->getFontBaseSize("boldfont");
}

void ToggleButton::draw() {
    Frame::draw();

    auto text = m_text + (m_flag ? ": ON" : ": OFF");
    RenderManager::drawText("boldfont", text, {getWidth() / 2, getHeight() / 2}, COL_WHITE, m_fontSize, {0.5f, 0.5f});
}

void ToggleButton::update() {
    auto sm = StyleManager::get();
    auto mouse = GetMousePosition();
    auto rect = getWorldBounds();
    m_color = sm->getValue<Col4u>(DEFAULT_UI_COLOR);

    if(rect.contains({mouse.x, mouse.y})) {
        m_color.brightness(-0.3f * (IsMouseButtonDown(MOUSE_BUTTON_LEFT) + 1));
    }

    if(rect.contains({mouse.x, mouse.y}) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        callback();
    }
}

void ToggleButton::callback() {
    m_flag = !m_flag;

    if (m_callback) {
        m_callback(this, m_flag);
    }
}
