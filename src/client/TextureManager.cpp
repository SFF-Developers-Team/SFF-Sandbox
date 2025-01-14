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

bool TextureManager::loadTileMap(std::filesystem::path const& filepath, Vec2i tileSize) {
    if(!std::filesystem::exists(filepath)) {
        return false;
    }

    auto key = filepath.filename().string();
    m_tilemaps[key] = std::make_shared<TileMap>(filepath, tileSize); 
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

std::shared_ptr<TileMap> TextureManager::getTileMap(std::string const& key) {
    return m_tilemaps[key];
}