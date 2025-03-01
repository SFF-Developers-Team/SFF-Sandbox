#include <ui/basic/Text.hpp>
#include <StyleManager.hpp>
#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <raylib.h>

Text::Text(std::string const& font, std::string const& text, float fontSize) : Node(), m_font(font), m_text(text), m_fontSize(fontSize), m_wordWrap(false) {
    auto sm = StyleManager::get();
    auto fnt = TextureManager::get()->getFont(m_font);

    if(fontSize == 0.f) {
        m_fontSize = fnt.baseSize;
    }

    auto size = MeasureTextEx(fnt, m_text.c_str(), m_fontSize, 1.f);
    setSize({size.x, size.y});
    m_alignHorizontal = H_CENTER;
    m_alignVertical = V_CENTER;
}

std::vector<std::string> Text::wrapText() {
    std::vector<std::string> lines;
    std::string currentLine;
    std::string currentWord;

    for (char ch : m_text) {
        if (ch == ' ') {
            auto wordWidth = RenderManager::getTextSize(currentLine + currentWord, m_font, m_fontSize).x;
            
            if (wordWidth > getWidth()) {
                lines.push_back(currentLine);
                currentLine = currentWord + " ";
            } else {
                currentLine += currentWord + " ";
            }
            currentWord.clear();
        } else {
            currentWord += ch;
        }
    }

    auto wordWidth = RenderManager::getTextSize(currentLine + currentWord, m_font, m_fontSize).x;
    if (wordWidth > getWidth()) {
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
    auto textpos = Vec2f {0.f, 0.f};

    if(m_wordWrap) {
        std::vector<std::string> lines = wrapText();

        float lineHeight = m_fontSize + 2;
        float totalHeight = lineHeight * lines.size();

        if(m_alignVertical == V_CENTER) textpos.y += getHeight() * 0.5f - totalHeight * 0.5f;
        if(m_alignVertical == V_BOTTOM) textpos.y += getHeight()- totalHeight;

        for (const auto& line : lines) {
            auto size = RenderManager::getTextSize(line, m_font, m_fontSize);

            if(m_alignHorizontal == H_CENTER) textpos.x = getWidth() * 0.5f - size.x * 0.5f;
            if(m_alignHorizontal == H_RIGHT) textpos.x = getWidth() - size.x;

            RenderManager::drawColoredText(m_font, line, textpos, m_color, m_fontSize);
            textpos.y += size.y + 2.f;

            if (textpos.y + lineHeight > getHeight()) return;
        }
    } else {
        auto size = RenderManager::getTextSize(m_text, m_font, m_fontSize);

        if(m_alignHorizontal == H_CENTER) textpos.x += getWidth() * 0.5f - size.x * 0.5f;
        if(m_alignHorizontal == H_RIGHT) textpos.x += getWidth() - size.x;

        if(m_alignVertical == V_CENTER) textpos.y += getHeight() * 0.5f - size.y * 0.5f;
        if(m_alignVertical == V_BOTTOM) textpos.y += getHeight() - size.y;

        RenderManager::drawColoredText(m_font, m_text, textpos, m_color, m_fontSize);
    }
}

void Text::setWordWrap(bool flag, bool autoHeight) {
    m_wordWrap = flag;

    if(autoHeight) {
        auto lines = wrapText();
        auto height = 0.f;

        for(auto& line : lines) {
            auto size = RenderManager::getTextSize(line, m_font, m_fontSize).y;
            height += size + 2.f;
        }

        setHeight(height);
    }
}

void Text::updateHeight() {
    auto height = 0.f;

    if (m_wordWrap) {
        auto lines = wrapText();

        for(auto& line : lines) {
            auto size = RenderManager::getTextSize(line, m_font, m_fontSize).y;
            height += size + 2.f;
        }
    } else {
        height = RenderManager::getTextSize(m_text, m_font, m_fontSize).y;
    }

    setHeight(height);
}

void Text::setText(std::string const& text) { 
    m_text = text;
    updateHeight();
}

void Text::append(std::string const& text) { 
    m_text.append(text); 
    updateHeight();
}