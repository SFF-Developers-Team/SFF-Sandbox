#include <ui/nodes/Texture.hpp>
#include <RenderManager.hpp>

Texture::Texture(std::string const& textureKey) : m_textureKey(textureKey) {}

void Texture::draw() {
    RenderManager::get()->drawTexture(m_textureKey, m_bounds, m_color);
}