#include <ui/nodes/Text.hpp>
#include <StyleManager.hpp>
#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <raylib.h>

Text::Text(std::string const& font, std::string const& text, float fontSize) : Node(), m_font(font), m_text(text), m_fontSize(fontSize), m_wordWrap(false) {
    auto sm = StyleManager::get();
    auto fnt = TextureManager::get()->getFont(m_font);
    auto size = MeasureTextEx(fnt, m_text.c_str(), m_fontSize, 1.f);
    m_bounds.width = size.x;
    m_bounds.height = size.y;
    m_color = sm->getValue<Col4u>(TEXT_COLOR_NORMAL);
    m_alignHorizontal = H_CENTER;
    m_alignVertical = V_CENTER;
}

std::vector<std::string> Text::wrapText() {
    auto rm = RenderManager::get();
    std::vector<std::string> lines;
    std::string currentLine;
    std::string currentWord;

    for (char ch : m_text) {
        if (ch == ' ') {
            // Проверяем, помещается ли текущее слово в текущую строку
            auto wordWidth = rm->getTextSize(currentLine + currentWord, m_font, m_fontSize).x;
            
            if (wordWidth > m_bounds.width) {
                // Если слово не помещается, начинаем новую строку
                lines.push_back(currentLine);
                currentLine = currentWord + " ";
            } else {
                // Добавляем слово к текущей строке
                currentLine += currentWord + " ";
            }
            currentWord.clear();
        } else {
            currentWord += ch;
        }
    }

    // Добавляем последнее слово
    auto wordWidth = rm->getTextSize(currentLine + currentWord, m_font, m_fontSize).x;
    if (wordWidth > m_bounds.width) {
        lines.push_back(currentLine);
        lines.push_back(currentWord);
    } else {
        lines.push_back(currentLine + currentWord);
    }

    return lines;
}

void Text::draw() {
    Node::draw();
    
    auto tm = TextureManager::get();
    auto rm = RenderManager::get();
    auto textpos = Vec2f {0.f, 0.f};

    if(m_wordWrap) {
        std::vector<std::string> lines = wrapText();

        float lineHeight = m_fontSize + 2;
        float totalHeight = lineHeight * lines.size();

        if(m_alignVertical == V_CENTER) textpos.y += m_bounds.height * 0.5f - totalHeight * 0.5f;
        if(m_alignVertical == V_BOTTOM) textpos.y += m_bounds.height - totalHeight;

        for (const auto& line : lines) {
            auto size = rm->getTextSize(line, m_font, m_fontSize);

            if(m_alignHorizontal == H_CENTER) textpos.x = m_bounds.width * 0.5f - size.x * 0.5f;
            if(m_alignHorizontal == H_RIGHT) textpos.x = m_bounds.width - size.x;

            rm->drawText(m_font, line, textpos, m_color, m_fontSize);
            textpos.y += size.y + 2.f;

            if (textpos.y + lineHeight > m_bounds.height) return;
        }
    } else {
       auto size = rm->getTextSize(m_text, m_font, m_fontSize);

        if(m_alignHorizontal == H_CENTER) textpos.x += m_bounds.width * 0.5f - size.x * 0.5f;
        if(m_alignHorizontal == H_RIGHT) textpos.x += m_bounds.width - size.x;

        if(m_alignVertical == V_CENTER) textpos.y += m_bounds.height * 0.5f - size.y * 0.5f;
        if(m_alignVertical == V_BOTTOM) textpos.y += m_bounds.height - size.y;

        rm->drawText(m_font, m_text, textpos, m_color, m_fontSize);
    }
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

void Text::setWordWrap(bool flag, bool autoHeight) {
    m_wordWrap = flag;

    if(autoHeight) {
        auto lines = wrapText();
        auto rm = RenderManager::get();
        auto height = 0.f;

        for(auto& line : lines) {
            auto size = rm->getTextSize(line, m_font, m_fontSize).y;
            height += size + 2.f;
        }

        setHeight(height);
    }
}

void Text::setAlignV(TextAlignmentV alignV) {
    m_alignVertical = alignV;
}

void Text::setAlignH(TextAlignmentH alignH) {
    m_alignHorizontal = alignH;
}