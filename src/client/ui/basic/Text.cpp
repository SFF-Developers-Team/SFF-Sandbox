#include <ui/basic/Text.hpp>
#include <managers/StyleManager.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/RenderManager.hpp>
#include <raylib-cpp.hpp>

Text::Text(std::string const& font, std::string const& text, float fontSize) : Node(), m_font(font), m_text(text), m_fontSize(fontSize), m_wordWrap(false) {
    auto sm = StyleManager::get();
    auto& fnt = ResourceManager::Get().GetFont(m_font);

    if(fontSize == 0.f) {
        m_fontSize = fnt.baseSize;
    }

    auto size = fnt.MeasureText(m_text, m_fontSize, 1.f);
    setSize({size.x, size.y});
    m_alignHorizontal = H_CENTER;
    m_alignVertical = V_CENTER;
}

std::vector<std::string> Text::wrapText() {
    std::vector<std::string> lines;
    std::string currentLine;
    std::string currentWord;

    auto& rm = ResourceManager::Get();

    for (char ch : m_text) {
        if (ch == ' ') {
            auto wordWidth = rm.GetFont(m_font).MeasureText(currentLine + currentWord, m_fontSize, 1.f).x;
            
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

    auto wordWidth = rm.GetFont(m_font).MeasureText(currentLine + currentWord, m_fontSize, 1.f).x;
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
    
    auto textpos = raylib::Vector2 {0.f, 0.f};
    auto& rm = ResourceManager::Get();

    if(m_wordWrap) {
        std::vector<std::string> lines = wrapText();

        float lineHeight = m_fontSize + 2;
        float totalHeight = lineHeight * lines.size();

        if(m_alignVertical == V_CENTER) textpos.y += getHeight() * 0.5f - totalHeight * 0.5f;
        if(m_alignVertical == V_BOTTOM) textpos.y += getHeight()- totalHeight;

        for (const auto& line : lines) {
            auto size = rm.GetFont(m_font).MeasureText(line, m_fontSize, 1.f);

            if(m_alignHorizontal == H_CENTER) textpos.x = getWidth() * 0.5f - size.x * 0.5f;
            if(m_alignHorizontal == H_RIGHT) textpos.x = getWidth() - size.x;

            rm.GetFont(m_font).DrawText(line, textpos, m_fontSize, 1.f, m_color);
            textpos.y += size.y + 2.f;

            if (textpos.y + lineHeight > getHeight()) return;
        }
    } else {
        auto size = rm.GetFont(m_font).MeasureText(m_text, m_fontSize, 1.f);

        if(m_alignHorizontal == H_CENTER) textpos.x += getWidth() * 0.5f - size.x * 0.5f;
        if(m_alignHorizontal == H_RIGHT) textpos.x += getWidth() - size.x;

        if(m_alignVertical == V_CENTER) textpos.y += getHeight() * 0.5f - size.y * 0.5f;
        if(m_alignVertical == V_BOTTOM) textpos.y += getHeight() - size.y;

        rm.GetFont(m_font).DrawText(m_text, textpos, m_fontSize, 1.f, m_color);
    }
}

void Text::setWordWrap(bool flag, bool autoHeight) {
    m_wordWrap = flag;

    if(autoHeight) {
        auto lines = wrapText();
        auto height = 0.f;

        for(auto& line : lines) {
            auto size = ResourceManager::Get().GetFont(m_font).MeasureText(line, m_fontSize, 1.f).y;
            height += size + 2.f;
        }

        setHeight(height);
    }
}

void Text::updateHeight() {
    auto height = 0.f;

    auto& rm = ResourceManager::Get();

    if (m_wordWrap) {
        auto lines = wrapText();

        for(auto& line : lines) {
            auto size = rm.GetFont(m_font).MeasureText(line, m_fontSize, 1.f).y;
            height += size + 2.f;
        }
    } else {
        height = rm.GetFont(m_font).MeasureText(m_text, m_fontSize, 1.f).y;
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