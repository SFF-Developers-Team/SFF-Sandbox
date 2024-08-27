#pragma once

#include "Node.hpp"
#include <string>

namespace sandbox_ui {
    class Text : public Node {
    protected:
        std::string m_str;
        Font m_font;

        float m_baseTextSize = 20.f;
        float m_spacing = 1.f;

        void updateTextSize();

        Node::Color m_shadowColor = {0,0,0,0};
        bool m_shadow = false;
    public:
        Text(const std::string &str);
    
        std::string getString();
        void setString(const std::string &str);

        void draw() override;

        void setFont(Font fnt);
        void setBaseTextSize(float sz);
        void setCharSpacing(float sp);

        void setScale(float sz) override;
        
        void enableShadow(bool flag, Node::Color col);
    };
}