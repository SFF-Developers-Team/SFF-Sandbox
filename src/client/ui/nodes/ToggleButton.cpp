#include <ui/nodes/ToggleButton.hpp>
#include <raylib.h>
#include <format>
#include <RenderManager.hpp>
#include <StyleManager.hpp>

ToggleButton::ToggleButton(std::string const& text, MiniFunction<void(ToggleButton*, bool)> const& callback) : 
    Frame(), m_flag(false), m_callback(callback), m_text(text), m_fontSize(40.f) {}

void ToggleButton::draw() {
    Frame::draw();

    auto text = m_text + (m_flag ? ": ON" : ": OFF");

    auto rm = RenderManager::get();
    auto textsize = rm->getTextSize(text, "boldfont", m_fontSize);

    rm->drawText("boldfont", text, {(m_bounds.width - textsize.x) * 0.5f, (m_bounds.height - m_fontSize) * 0.5f}, COL_WHITE, m_fontSize);
}

void ToggleButton::update() {
    auto sm = StyleManager::get();
    auto mouse = GetMousePosition();
    auto rect = getRealBounds();
    m_color = sm->getValue<Col4u>(FIRST_COLOR_NORMAL);

    if(rect.contains({mouse.x, mouse.y})) {
        m_color.brightness(-0.3f * (IsMouseButtonDown(MOUSE_BUTTON_LEFT) + 1));
    }

    if(rect.contains({mouse.x, mouse.y}) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_flag = !m_flag;
        m_callback(this, m_flag);
    }
}

bool ToggleButton::getValue() {
    return m_flag;
}

void ToggleButton::setValue(bool flag) {
    m_flag = flag;
}

void ToggleButton::setText(std::string const& text) {
    m_text = text;
}