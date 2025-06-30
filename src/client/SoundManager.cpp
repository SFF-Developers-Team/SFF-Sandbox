#include "SoundManager.hpp"

raylib::Sound& SoundManager::LoadSound(std::filesystem::path const& path) {
    auto const fullPath = (!path.is_absolute()) ? "assets" / path : path;
    if(!std::filesystem::exists(fullPath)) {
        throw std::filesystem::filesystem_error(
            "Sound not found: ", 
            fullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }
    auto key = fullPath.filename().string();
    m_sounds[key] = raylib::Sound(fullPath);

    return m_sounds[key];
}

raylib::Music& SoundManager::LoadMusic(std::filesystem::path const& path) {
    auto const fullPath = (!path.is_absolute()) ? "assets" / path : path;
    if(!std::filesystem::exists(fullPath)) {
        throw std::filesystem::filesystem_error(
            "Sound not found: ", 
            fullPath, 
            std::make_error_code(std::errc::no_such_file_or_directory)
        );
    }
    auto key = fullPath.filename().string();
    m_soundtracks[key] = raylib::Music(fullPath);

    return m_soundtracks[key];
}

raylib::Sound& SoundManager::GetSound(std::string const& filename) {
    return m_sounds[filename];
}

raylib::Music& SoundManager::GetMusic(std::string const& filename) {
    return m_soundtracks[filename];
}