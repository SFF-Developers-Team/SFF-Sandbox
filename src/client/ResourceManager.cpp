#include "ResourceManager.hpp"
#include "Tilemap.hpp"
#include <Image.hpp>
#include <Music.hpp>
#include <Texture.hpp>
#include <filesystem>
#include <stdexcept>

ResourceManager::ResourceManager() {
    raylib::Image missing(2, 2, BLACK);
    missing.DrawPixel({0, 0}, PURPLE);
    missing.DrawPixel({1, 1}, PURPLE);

    m_textures["missing"] = missing.LoadTexture();
}

std::filesystem::path ResourceManager::GetAssetsPath(std::filesystem::path const& path) {
    return (!path.is_absolute()) ? "assets" / path : path;
}

raylib::Texture2D& ResourceManager::LoadTexture(std::filesystem::path const& path) {
    auto const fullPath = GetAssetsPath(path);

    if(!std::filesystem::exists(fullPath)) {
        throw std::filesystem::filesystem_error(
            "Texture not found", fullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    
    auto const key = fullPath.filename().string();

    if (auto it = m_textures.find(key); it != m_textures.end()) {
        return it->second;
    }

    try {
        auto texture = raylib::Image(fullPath).LoadTexture();
        auto [it, inserted] = m_textures.try_emplace(key, std::move(texture));

        return it->second;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load texture: " + key + " (" + e.what() + ")");
    }
}

raylib::Texture2D& ResourceManager::GetTexture(std::string const& filename) {
    if (auto it = m_textures.find(filename); it != m_textures.end()) {
        return it->second;
    }

    throw std::runtime_error("Texture " + filename + " not found!");
}

Tilemap& ResourceManager::LoadTilemap(std::filesystem::path const& path, int tileCountX, int tileCountY) {
    auto const key = path.filename().string();

    auto it = m_tilemaps.find(key);
    
    if (it != m_tilemaps.end()) {
        return it->second;
    }

    raylib::Texture2D& texture = [&]() -> raylib::Texture2D& {
        try {
            return LoadTexture(path);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to load texture for tilemap: " + key + " (" + e.what() + ")");
        }
    }();

    try {
        auto [it, inserted] = m_tilemaps.try_emplace(
            key,
            texture, tileCountX, tileCountY
        );

        return it->second;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to create tilemap: " + key + " (" + e.what() + ")");
    }
}

Tilemap& ResourceManager::GetTilemap(std::string const& filename) {
    if (auto it = m_tilemaps.find(filename); it != m_tilemaps.end()) {
        return it->second;
    }

    throw std::runtime_error("Tilemap " + filename + " not found!");
}

raylib::Sound& ResourceManager::LoadSound(std::filesystem::path const& path) {
    auto const fullPath = GetAssetsPath(path);

    if(!std::filesystem::exists(fullPath)) {
        throw std::filesystem::filesystem_error(
            "Sound not found", fullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    auto const key = fullPath.string();

    if (auto it = m_sounds.find(key); it != m_sounds.end()) {
        return it->second;
    }

    try {
        raylib::Sound sound(fullPath.string());
        auto [it, inserted] = m_sounds.try_emplace(key, std::move(sound));
        
        return it->second;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load sound " + key + ": " + e.what());
    }
}

raylib::Sound& ResourceManager::GetSound(std::string const& filename) {
    if (auto it = m_sounds.find(filename); it != m_sounds.end()) {
        return it->second;
    }

    throw std::runtime_error("Sound " + filename + " not found!");
}

raylib::Music& ResourceManager::LoadMusic(std::filesystem::path const& path) {
    auto const fullPath = GetAssetsPath(path);

    if(!std::filesystem::exists(fullPath)) {
        throw std::filesystem::filesystem_error(
            "Sound not found", fullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    auto const key = fullPath.string();

    if (auto it = m_soundtracks.find(key); it != m_soundtracks.end()) {
        return it->second;
    }

    try {
        raylib::Music soundtrack(fullPath.string());
        auto [it, inserted] = m_soundtracks.try_emplace(key, std::move(soundtrack));
        
        return it->second;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load soundtrack " + key + ": " + e.what());
    }
}

raylib::Music& ResourceManager::GetMusic(std::string const& filename) {
    if (auto it = m_soundtracks.find(filename); it != m_soundtracks.end()) {
        return it->second;
    }

    throw std::runtime_error("Soundtrack " + filename + " not found!");
}

void ResourceManager::UpdateMusic() {
    for (auto& [_, music] : m_soundtracks) {
        if (music.IsPlaying()) {
            music.Update();
        }
    }
}

raylib::Shader& ResourceManager::LoadShader(std::filesystem::path const& vsPath, std::filesystem::path const& fsPath) {
    auto const vsFullPath = GetAssetsPath(vsPath);
    auto const fsFullPath = GetAssetsPath(fsPath);

    if(!std::filesystem::exists(vsFullPath)) {
        throw std::filesystem::filesystem_error(
            "Shader not found", vsFullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    if(!std::filesystem::exists(fsFullPath)) {
        throw std::filesystem::filesystem_error(
            "Shader not found", fsFullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }

    auto const key = vsFullPath.stem().string();

    if (auto it = m_shaders.find(key); it != m_shaders.end()) {
        return it->second;
    }

    try {
        raylib::Shader shader(vsFullPath.string(), fsFullPath.string());
        auto [it, inserted] = m_shaders.try_emplace(key, std::move(shader));
        
        return it->second;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to load shader " + key + ": " + e.what());
    }
}

raylib::Shader& ResourceManager::GetShader(std::string const& key) {
    if (auto it = m_shaders.find(key); it != m_shaders.end()) {
        return it->second;
    }

    throw std::runtime_error("Shader " + key + " not found!");
}