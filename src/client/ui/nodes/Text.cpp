#include <ui/nodes/Text.hpp>
#include <StyleManager.hpp>
#include <raylib.h>

Text::Text(std::string const& text, float size) : Node(), m_text(text), m_size(size) {
    m_bounds.width = 0;
    m_bounds.height = 0;
}

void Text::draw() {
    auto width = MeasureText(m_text.c_str(), m_size);
    auto sm = StyleManager::get();
    auto color = sm->getValue<Col4u>(TEXT_COLOR_NORMAL).to<Color>();

    DrawText(m_text.c_str(), m_bounds.x - width * m_anchor.x, m_bounds.y - m_size * m_anchor.y, m_size, color);
}

void Text::setText(std::string const& text) {
    m_text = text;
}