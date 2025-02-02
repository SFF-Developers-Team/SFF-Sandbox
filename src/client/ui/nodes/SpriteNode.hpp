#pragma once
#include <ui/nodes/Node.hpp>

class SpriteNode : public Node {
protected:
    std::string m_textureKey;

public:
    SpriteNode(std::string const& textureKey);

    void draw();
};