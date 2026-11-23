#include <ui/basic/SpriteNode.hpp>
#include <managers/RenderManager.hpp>
#include <managers/ResourceManager.hpp>

SpriteNode::SpriteNode(std::string const& textureKey) : m_textureKey(textureKey) {}

void SpriteNode::draw() {
    auto& rm = ResourceManager::Get();
    auto& texture = rm.GetTexture(m_textureKey);
    texture.Draw(raylib::Rectangle{texture.GetSize()}, raylib::Rectangle{getSize()}, raylib::Vector2::Zero(), 0.f, m_color);
}