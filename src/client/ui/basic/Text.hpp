#pragma once
#include <ui/basic/Node.hpp>

enum TextAlignmentV : uint8_t {
    V_TOP, V_CENTER, V_BOTTOM
};


enum TextAlignmentH : uint8_t {
    H_LEFT, H_CENTER, H_RIGHT
};

class Text : public Node {
protected:
    TextAlignmentV m_alignVertical;
    TextAlignmentH m_alignHorizontal;
    std::string m_font;
    std::string m_text;
    float m_fontSize;
    bool m_wordWrap;

    std::vector<std::string> wrapText();

public:
    Text(std::string const& font, std::string const& text, float size = 0.f);

    void draw();

    float getFontSize() { return m_fontSize; }
    void setFontSize(float fontSize) { m_fontSize = fontSize; }

    std::string const& getText() { return m_text; }
    void setText(std::string const& text);
    void append(std::string const& text);

    std::string const& getFont() { return m_font; }
    void setFont(std::string const& font) { m_font = font; }

    void setWordWrap(bool flag, bool autoHeight);

    void setAlignV(TextAlignmentV alignV) { m_alignVertical = alignV; }
    void setAlignH(TextAlignmentH alignH) { m_alignHorizontal = alignH; }

    void updateHeight();
};