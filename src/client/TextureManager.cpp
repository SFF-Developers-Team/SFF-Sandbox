#include <TextureManager.hpp>

bool TextureManager::loadTexture(std::filesystem::path const& filepath) {
    if(!std::filesystem::exists(filepath)) {
        return false;
    }

    auto key = filepath.filename().string();
    auto texture = LoadTexture(filepath.string().c_str());
    m_textures[key] = texture;

    return true;
}

void TextureManager::unloadTexture(std::string const& key) {
    if(m_textures.contains(key)) {
        UnloadTexture(m_textures[key]);
        m_textures.erase(key);
    }
}

Texture2D& TextureManager::getTexture(std::string const& key) {
    return m_textures[key];
}

bool TextureManager::textureLoaded(std::string const& key) {
    return m_textures.contains(key);
}