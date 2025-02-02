#include <ui/nodes/SpriteNode.hpp>
#include <RenderManager.hpp>

SpriteNode::SpriteNode(std::string const& textureKey) : m_textureKey(textureKey) {}

void SpriteNode::draw() {
    RenderManager::get()->drawTexture(m_textureKey, {0, 0, m_bounds.width, m_bounds.height}, m_color);
}