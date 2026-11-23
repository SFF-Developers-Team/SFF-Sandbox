#include <managers/ResourceManager.hpp>
#include <Image.hpp>

#include <algorithm>
#include <stdexcept>
#include <system_error>
#include <utility>

ResourceManager::ResourceManager() {
    raylib::Image missing(2, 2, raylib::Color::Black());
    missing.DrawPixel({0, 0}, raylib::Color::Purple());
    missing.DrawPixel({1, 1}, raylib::Color::Purple());
    m_textures.try_emplace("missing", missing);
}

std::filesystem::path ResourceManager::GetAssetsPath(const std::filesystem::path& path) {
#ifdef PLATFORM_ANDROID
    return path;
#else
    return path.is_absolute() ? path : std::filesystem::path("assets") / path;
#endif
}

std::string ResourceManager::GetFileKey(const std::filesystem::path& path) {
    return path.filename().string();
}

raylib::Texture2D& ResourceManager::LoadTexture(const std::filesystem::path& path) {
    const auto key = GetFileKey(path);
    if (auto it = m_textures.find(key); it != m_textures.end()) return it->second;
    const auto fullPath = GetAssetsPath(path);
    auto [it, inserted] = m_textures.try_emplace(key, fullPath.string());
    return it->second;
}

raylib::Texture2D& ResourceManager::GetTexture(const std::string& key) {
    return m_textures.at(key);
}

Tilemap& ResourceManager::LoadTilemap(const std::filesystem::path& path, int columns, int rows) {
    const auto key = GetFileKey(path);
    if (auto it = m_tilemaps.find(key); it != m_tilemaps.end()) return it->second;
    auto& texture = LoadTexture(path);
    auto [it, inserted] = m_tilemaps.try_emplace(key, texture, columns, rows);
    return it->second;
}

Tilemap& ResourceManager::GetTilemap(const std::string& key) {
    return m_tilemaps.at(key);
}

raylib::Font& ResourceManager::LoadFont(const std::filesystem::path& path) {
    // Fonts are requested as "font" and "boldfont" throughout the UI.
    const auto key = path.stem().string();
    if (auto it = m_fonts.find(key); it != m_fonts.end()) return it->second;
    auto [it, inserted] = m_fonts.try_emplace(key, GetAssetsPath(path).string());
    return it->second;
}

raylib::Font& ResourceManager::GetFont(const std::string& key) {
    return m_fonts.at(key);
}

int ResourceManager::GetFontBaseSize(const std::string& key) {
    return GetFont(key).baseSize;
}

raylib::Sound& ResourceManager::LoadSound(const std::filesystem::path& path) {
    const auto key = GetFileKey(path);
    if (auto it = m_sounds.find(key); it != m_sounds.end()) return it->second;
    auto [it, inserted] = m_sounds.try_emplace(key, GetAssetsPath(path).string());
    it->second.SetVolume(m_soundVolume);
    return it->second;
}

raylib::Sound& ResourceManager::GetSound(const std::string& key) {
    return m_sounds.at(key);
}

raylib::Music& ResourceManager::LoadMusic(const std::filesystem::path& path) {
    const auto key = GetFileKey(path);
    if (auto it = m_music.find(key); it != m_music.end()) return it->second;
    auto [it, inserted] = m_music.try_emplace(key, GetAssetsPath(path).string());
    it->second.SetVolume(m_musicVolume);
    return it->second;
}

raylib::Music& ResourceManager::GetMusic(const std::string& key) {
    return m_music.at(key);
}

void ResourceManager::SetSoundVolume(float volume) {
    m_soundVolume = std::clamp(volume, 0.f, 1.f);
    for (auto& [key, sound] : m_sounds) sound.SetVolume(m_soundVolume);
}

void ResourceManager::SetMusicVolume(float volume) {
    m_musicVolume = std::clamp(volume, 0.f, 1.f);
    for (auto& [key, music] : m_music) music.SetVolume(m_musicVolume);
}

void ResourceManager::UpdateMusic() {
    for (auto& [key, music] : m_music) {
        if (music.IsPlaying()) music.Update();
    }
}

void ResourceManager::Clear() {
    m_tilemaps.clear(); // Holds non-owning texture references.
    m_music.clear();
    m_sounds.clear();
    m_fonts.clear();
    m_textures.clear();
}
