#pragma once

#include "Node.hpp"
#include "ContainerizedNode.hpp"

namespace sandbox_ui {
    class TomlNode : public Node, public ContainerizedNode {
    public:
        TomlNode(const std::string &filepath);
        
        void draw() override;
        void update() override;
    };
}