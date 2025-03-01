#pragma once
#include <ui/basic/Node.hpp>

class Tile : public Node {
protected:
    std::string m_tilemapKey;
    int m_index;

public:
    Tile(std::string const& tilemapKey, int index);

    void draw();
};