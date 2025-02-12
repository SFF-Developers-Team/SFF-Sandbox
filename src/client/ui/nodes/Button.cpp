#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <raylib.h>
#include <StyleManager.hpp>
#include <RenderManager.hpp>

Button::Button(std::string const& text, MiniFunction<void(Button*)> const& callback) : Frame(), m_text(text), m_callback(callback), m_fontSize(40.f) {
    setWidth(400.f);
    setHeight(40.f);
}

void Button::update() {
    auto sm = StyleManager::get();
    auto mouse = GetMousePosition();
    auto rect = getWorldBounds();
    m_color = sm->getValue<Col4u>(FIRST_COLOR_NORMAL);

    if(rect.contains({mouse.x, mouse.y})) {
        m_color.brightness(-0.3f * (IsMouseButtonDown(MOUSE_BUTTON_LEFT) + 1));
    }

    if(rect.contains({mouse.x, mouse.y}) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_callback(this);
    }
}

void Button::draw() {
    Frame::draw();

    auto rm = RenderManager::get();
    auto textsize = rm->getTextSize(m_text, "boldfont", m_fontSize);

    rm->drawText("boldfont", m_text, {(m_bounds.width - textsize.x) * 0.5f, (m_bounds.height - m_fontSize) * 0.5f}, COL_WHITE, m_fontSize);
}

void Button::setText(std::string const& text) {
    m_text = text;
}