#pragma once
#include <ui/nodes/Node.hpp>

class TextInput : public Node {
protected:
    std::string m_text;
    std::string m_allowedChars;
    int m_maxChars = 128;
    int m_cursorX = 0;
    bool m_active = false;
    // float m_activeTime = 0.f;

public:
    TextInput();

    void draw();
    void update();

    std::string const& getText();
    int getLength();
};