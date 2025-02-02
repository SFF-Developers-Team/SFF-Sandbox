#include <ui/nodes/Tile.hpp>
#include <RenderManager.hpp>

Tile::Tile(std::string const& tilemapKey, int index) : m_tilemapKey(tilemapKey), m_index(index) {}

void Tile::draw() {
    Node::draw();
    RenderManager::get()->drawTile(m_tilemapKey, m_index, {0, 0, m_bounds.width, m_bounds.height}, m_color);
}