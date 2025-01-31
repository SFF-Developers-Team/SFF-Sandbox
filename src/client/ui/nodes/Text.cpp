#include <ui/nodes/Text.hpp>
#include <StyleManager.hpp>
#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <raylib.h>

Text::Text(std::string const& font, std::string const& text, float fontSize) : Node(), m_font(font), m_text(text), m_fontSize(fontSize) {
    auto sm = StyleManager::get();
    auto fnt = TextureManager::get()->getFont(m_font);
    auto size = MeasureTextEx(fnt, m_text.c_str(), m_fontSize, 1.f);
    m_bounds.width = size.x;
    m_bounds.height = size.y;
    m_color = sm->getValue<Col4u>(TEXT_COLOR_NORMAL);
}

void Text::draw() {
    auto tm = TextureManager::get();
    auto rm = RenderManager::get();
    auto size = MeasureTextEx(tm->getFont(m_font), m_text.c_str(), m_fontSize, 1.f);
    auto textpos = Vec2f {0.f, 0.f};

    if(m_alignHorizontal == H_CENTER) textpos.x += m_bounds.width * 0.5f - size.x * 0.5f;
    if(m_alignHorizontal == H_RIGHT) textpos.x += m_bounds.width - size.x;

    if(m_alignVertical == V_CENTER) textpos.y += m_bounds.height * 0.5f - size.y * 0.5f;
    if(m_alignVertical == V_DOWN) textpos.y += m_bounds.height - size.y;

    rm->drawText(m_font, m_text, textpos, m_color, m_fontSize, 1.f);
}

float Text::getFontSize() {
    return m_fontSize;
}

void Text::setFontSize(float fontSize) {
    m_fontSize = fontSize;
}

std::string const& Text::getText() {
    return m_text;
}

void Text::setText(std::string const& text) {
    m_text = text;
}

std::string const& Text::getFont() {
    return m_font;
}

void Text::setFont(std::string const& font) {
    m_font = font;
}