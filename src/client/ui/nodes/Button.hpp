#pragma once
#include <string>
#include <ui/nodes/Node.hpp>
#include <MiniFunction.hpp>

class Button : public Node {
protected:
    std::string m_text;
    MiniFunction<void()> m_callback;

public:
    Button(std::string const& text, MiniFunction<void()> const& callback);

    void draw() override;
    void update() override;
};