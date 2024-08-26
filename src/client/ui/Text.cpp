#include "Text.hpp"

sandbox_ui::Text::Text(const std::string &str) {
    m_font = GetFontDefault();

    setString(str);
}

std::string sandbox_ui::Text::getString() {
    return m_str;
}
void sandbox_ui::Text::setString(const std::string &str) {
    m_str = str;

    updateTextSize();
}

void sandbox_ui::Text::draw() {
    DrawTextEx(m_font, m_str.c_str(), getPosition(), m_baseTextSize * getScale(), m_spacing, getColor());
}

void sandbox_ui::Text::updateTextSize() {
    Vector2 text_sz = MeasureTextEx(m_font, m_str.c_str(), m_baseTextSize * getScale(), m_spacing);

    m_nodeRect.width = text_sz.x;
    m_nodeRect.height = text_sz.y;
}

void sandbox_ui::Text::setFont(Font fnt) {
    m_font = fnt;

    updateTextSize();
}

void sandbox_ui::Text::setBaseTextSize(float sz) {
    m_baseTextSize = sz;
}
void sandbox_ui::Text::setCharSpacing(float sp) {
    m_spacing = sp;
}

void sandbox_ui::Text::setScale(float sz) {
    Node::setScale(sz);
    updateTextSize();
}