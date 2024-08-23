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
        void setSize(float width, float height);
        void setSize(Pos sz);

        void setWidth(float width);
        void setHeight(float height);

        Node::Rect getRenderableRectangle();

        void draw() override;

        void setThickness(float v);

        // outlined mode does not support rotation
        void setMode(Mode mode);

        bool collides(sandbox_ui::Rectangle *ui_rect);
        bool collides(Pos point);
        bool collides(Node::Rect rl_rect);
    };
}