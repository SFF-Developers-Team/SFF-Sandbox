#include "Text.hpp"

sandbox_ui::Text::Text(const std::string &str) {
    setString(str);

    m_font = GetFontDefault();
}

std::string sandbox_ui::Text::getString() {
    return m_str;
}
void sandbox_ui::Text::setString(const std::string &str) {
    m_str = str;
}

void sandbox_ui::Text::draw() {
    DrawTextEx(m_font, m_str.c_str(), getPosition(), 20.f * getScale(), 1.f, getColor());
}