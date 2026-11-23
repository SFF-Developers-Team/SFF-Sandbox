#include <ui/basic/TileNode.hpp>
#include <managers/ResourceManager.hpp>

TileNode::TileNode(std::string const& tilemapKey, int index) : m_tilemapKey(tilemapKey), m_index(index) {}

void TileNode::draw() {
    Node::draw();

    auto& rm = ResourceManager::Get();
    rm.GetTilemap(m_tilemapKey).DrawTile(m_index, getSize(), m_color);
}