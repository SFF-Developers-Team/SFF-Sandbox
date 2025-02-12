#pragma once
#include <ui/nodes/Frame.hpp>

#define DEFAULT_ALLOWED_CHARS " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
#define URL_ALLOWED_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~!*'();/?:@&=+$ "

class TextInput : public Frame {
protected:
    std::string m_font;
    std::string m_text;
    std::string m_placeholder;
    std::string m_allowedChars;
    int m_maxChars = 128;
    int m_cursorX = 0;
    float m_renderOff = 0.f;
    bool m_active = false;
    // float m_activeTime = 0.f;

public:
    TextInput(std::string const& font, std::string const& placeholder);

    void draw();
    void update();

    std::string const& getText();
    int getLength();

    void setAllowedChars(std::string const& allowedChars);
};