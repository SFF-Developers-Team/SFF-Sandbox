#pragma once

#include "Node.hpp"

namespace sandbox_ui {
    class Rectangle : public Node {
    public:
        enum Mode {
            Filled,
            Outlined
        };
    protected:
        Mode m_rectMode = Mode::Filled;
        float m_thickness = 1.f;
    public:
        Rectangle();
        Rectangle(Node::Rect r);

        Node::Rect getRenderableRectangle();

        void draw() override;

        void setThickness(float v);

        // outlined mode does not support rotation
        void setMode(Mode mode);

        bool collides(sandbox_ui::Rectangle *ui_rect);
        bool collides(Pos point);
        bool collides(Node::Rect rl_rect);

        void modifyParam(const std::string &param, double value) override;
    };
}