#include <ui/basic/Tile.hpp>
#include <managers/RenderManager.hpp>

Tile::Tile(std::string const& tilemapKey, int index) : m_tilemapKey(tilemapKey), m_index(index) {}

void Tile::draw() {
    Node::draw();
    RenderManager::drawTile(m_tilemapKey, m_index, {0, 0, getWidth(), getHeight()}, m_color);
}