#include <ui/basic/TileNode.hpp>
#include <managers/RenderManager.hpp>

TileNode::TileNode(std::string const& tilemapKey, int index) : m_tilemapKey(tilemapKey), m_index(index) {}

void TileNode::draw() {
    Node::draw();
    RenderManager::drawTile(m_tilemapKey, m_index, {0, 0, getWidth(), getHeight()}, m_color);
}