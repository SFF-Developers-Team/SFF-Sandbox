#pragma once
#include <ui/nodes/Node.hpp>

class Texture : public Node {
protected:
    std::string m_textureKey;

public:
    Texture(std::string const& textureKey);

    void draw();
};