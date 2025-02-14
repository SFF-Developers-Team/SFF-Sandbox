#include <ui/nodes/SpriteNode.hpp>
#include <RenderManager.hpp>

SpriteNode::SpriteNode(std::string const& textureKey) : m_textureKey(textureKey) {}

void SpriteNode::draw() {
    RenderManager::get()->drawTexture(m_textureKey, {0, 0, getWidth(), getHeight()}, m_color);
}