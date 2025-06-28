#include "ResourceManager.hpp"
#include "Tilemap.hpp"
#include <Texture.hpp>
#include <filesystem>

raylib::Texture2D& ResourceManager::LoadTexture(std::filesystem::path const& path) {
    auto const fullPath = (!path.is_absolute()) ? "assets" / path : path;

    if(!std::filesystem::exists(fullPath)) {
        throw std::filesystem::filesystem_error(
            "Texture not found: ", 
            fullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    auto key = fullPath.filename().string();
    m_textures[key] = raylib::Image(fullPath).LoadTexture();

    return m_textures[key];
}

raylib::Texture2D& ResourceManager::GetTexture(std::string const& filename) {
    return m_textures[filename];
}

bool ResourceManager::LoadTilemap(std::filesystem::path const& path, int tileCountX, int tileCountY) {
    auto key = path.filename().string();

    if (!m_textures.contains(key)) {
        LoadTexture(path);
    }

    m_tilemaps.emplace(key, Tilemap(m_textures.at(key), tileCountX, tileCountY));

    return true;
}

Tilemap& ResourceManager::GetTilemap(std::string const& filename) {
    return m_tilemaps.at(filename);
}