#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <raylib.h>
#include <StyleManager.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>

Button::Button(std::string const& text, MiniFunction<void(Button*)> const& callback) : Frame(), m_text(text), m_callback(callback) {
    m_fontSize = TextureManager::get()->getFontBaseSize("boldfont");
}

void Button::update() {
    auto sm = StyleManager::get();
    auto mouse = getLocalMousePosition();
    m_color = sm->getValue<Col4u>(DEFAULT_UI_COLOR);

    // printf("%f %f | %f %f %f %f\n", mouse.x, mouse.y, m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height);

    const auto localRect = Rectf {0.f, 0.f, m_bounds.width, m_bounds.height};

    if(localRect.contains({mouse.x, mouse.y})) {
        m_color.brightness(-0.3f * (IsMouseButtonDown(MOUSE_BUTTON_LEFT) + 1));
    }

    if(localRect.contains({mouse.x, mouse.y}) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_callback(this);
    }
}

void Button::draw() {
    Frame::draw();
    auto rm = RenderManager::get();

    // debug
    auto mouse = getLocalMousePosition();
    rm->drawRect({mouse.x, mouse.y, 5.f, 5.f}, COL_YELLOW);
    rm->drawText("boldfont", m_text, {getWidth() / 2, getHeight() / 2}, COL_WHITE, m_fontSize, {0.5f, 0.5f});
}

void Button::setText(std::string const& text) {
    m_text = text;
}