#pragma once
#include <string>
#include <ui/nodes/Frame.hpp>
#include <MiniFunction.hpp>

class Button : public Frame {
protected:
    MiniFunction<void()> m_callback;
    std::string m_text;
    float m_fontSize;

public:
    Button(std::string const& text, MiniFunction<void()> const& callback);

    void draw();
    void update();
};