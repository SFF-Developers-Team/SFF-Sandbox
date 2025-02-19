#pragma once
#include <string>
#include <ui/nodes/Frame.hpp>
#include <MiniFunction.hpp>

class Button : public Frame {
protected:
    MiniFunction<void(Button*)> m_callback;
    std::string m_text;
    float m_fontSize;

public:
    Button(std::string const& text, MiniFunction<void(Button*)> const& callback);

    void draw();
    void update();

    void setText(std::string const& text);
};