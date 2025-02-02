#pragma once
#include <ui/nodes/Node.hpp>

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
    Text(std::string const& font, std::string const& text, float size = 11.f);

    void draw();

    float getFontSize();
    void setFontSize(float fontSize);

    std::string const& getText();
    void setText(std::string const& text);

    std::string const& getFont();
    void setFont(std::string const& font);

    void setWordWrap(bool flag, bool autoHeight);

    void setAlignV(TextAlignmentV alignV);
    void setAlignH(TextAlignmentH alignH);
};