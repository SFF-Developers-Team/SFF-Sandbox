#pragma once

#include <Tilemap.hpp>
#include <Font.hpp>
#include <Music.hpp>
#include <Sound.hpp>
#include <Texture.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
    static ResourceManager& Get() {
        static ResourceManager manager;
        return manager;
    }

    raylib::Texture2D& LoadTexture(const std::filesystem::path& path);
    [[nodiscard]] raylib::Texture2D& GetTexture(const std::string& key);

    // Counts of sprites in the sheet, not pixel sizes.
    Tilemap& LoadTilemap(const std::filesystem::path& path, int columns, int rows);
    [[nodiscard]] Tilemap& GetTilemap(const std::string& key);

    raylib::Font& LoadFont(const std::filesystem::path& path);
    [[nodiscard]] raylib::Font& GetFont(const std::string& key);
    [[nodiscard]] int GetFontBaseSize(const std::string& key);

    raylib::Sound& LoadSound(const std::filesystem::path& path);
    [[nodiscard]] raylib::Sound& GetSound(const std::string& key);
    raylib::Music& LoadMusic(const std::filesystem::path& path);
    [[nodiscard]] raylib::Music& GetMusic(const std::string& key);

    void SetSoundVolume(float volume);
    void SetMusicVolume(float volume);
    void UpdateMusic();
    // Must be called before the audio device and window are closed.
    void Clear();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

private:
    ResourceManager();

    static std::filesystem::path GetAssetsPath(const std::filesystem::path& path);
    static std::string GetFileKey(const std::filesystem::path& path);

    std::unordered_map<std::string, raylib::Texture2D> m_textures;
    std::unordered_map<std::string, Tilemap> m_tilemaps;
    std::unordered_map<std::string, raylib::Font> m_fonts;
    std::unordered_map<std::string, raylib::Sound> m_sounds;
    std::unordered_map<std::string, raylib::Music> m_music;
    float m_soundVolume = 1.f;
    float m_musicVolume = 1.f;
};
