#include <TextureManager.hpp>

bool TextureManager::loadTexture(std::filesystem::path const& filepath) {
    if(!std::filesystem::exists(filepath)) {
        return false;
    }

    auto key = filepath.filename().string();
    m_textures[key] = LoadTexture(filepath.string().c_str());

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

bool TextureManager::loadFont(std::filesystem::path const& filepath)  {
    if(!std::filesystem::exists(filepath)) {
        return false;
    }

    auto key = filepath.filename().stem().string();
    m_fonts[key] = LoadFont(filepath.string().c_str());

    return true;
}


void TextureManager::unloadTexture(std::string const& key) {
    if(m_textures.contains(key)) {
        UnloadTexture(m_textures[key]);
        m_textures.erase(key);
    }
}

void TextureManager::unloadFont(std::string const& key) {
    if(m_fonts.contains(key)) {
        UnloadFont(m_fonts[key]);
        m_fonts.erase(key);
    }
}


Texture2D& TextureManager::getTexture(std::string const& key) {
    return m_textures[key];
}

Font& TextureManager::getFont(std::string const& key) {
    return m_fonts[key];
}

bool TextureManager::textureLoaded(std::string const& key) {
    return m_textures.contains(key);
}

std::shared_ptr<TileMap> TextureManager::getTileMap(std::string const& key) {
    return m_tilemaps[key];
}

float TextureManager::getFontBaseSize(std::string const& fontKey) {
    return m_fonts[fontKey].baseSize;
}