#pragma once

#include "Node.hpp"
#include <string>

namespace sandbox_ui {
    class Text : public Node {
    protected:
        std::string m_str;
        Font m_font;
    public:
        Text(const std::string &str);
    
        std::string getString();
        void setString(const std::string &str);

        void draw() override;
    };
}