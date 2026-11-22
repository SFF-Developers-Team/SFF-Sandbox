#pragma once
#include <ui/basic/Node.hpp>

class TileNode : public Node {
protected:
    std::string m_tilemapKey;
    int m_index;

public:
    TileNode(std::string const& tilemapKey, int index);

    void draw();
};