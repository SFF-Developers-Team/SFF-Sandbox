#pragma once

#include "Tilemap.hpp"
#include <Music.hpp>
#include <Shader.hpp>
#include <Sound.hpp>
#include <filesystem>
#include <unordered_map>

class ResourceManager { 
public:
    static inline ResourceManager& Get() {
        static ResourceManager rm;
        return rm;
    }

    raylib::Texture2D& LoadTexture(std::filesystem::path const& path);
    [[nodiscard]] raylib::Texture2D& GetTexture(std::string const& filename);

    Tilemap& LoadTilemap(std::filesystem::path const& path, int tileCountX, int tileCountY);
    [[nodiscard]] Tilemap& GetTilemap(std::string const& filename);

    raylib::Sound& LoadSound(std::filesystem::path const& path);
    [[nodiscard]] raylib::Sound& GetSound(std::string const& key);

    raylib::Music& LoadMusic(std::filesystem::path const& path);
    [[nodiscard]] raylib::Music& GetMusic(std::string const& key);

    raylib::Shader& LoadShader(std::filesystem::path const& vsPath, std::filesystem::path const& fsPath);
    [[nodiscard]] raylib::Shader& GetShader(std::string const& key);

    void UpdateMusic();

    ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&) = delete;
    
private:
    ResourceManager();

    std::filesystem::path GetAssetsPath(std::filesystem::path const& path);

    std::unordered_map<std::string, raylib::Texture2D> m_textures;
    std::unordered_map<std::string, Tilemap> m_tilemaps;

    std::unordered_map<std::string, raylib::Sound> m_sounds;
    std::unordered_map<std::string, raylib::Music> m_soundtracks;

    std::unordered_map<std::string, raylib::Shader> m_shaders;
};