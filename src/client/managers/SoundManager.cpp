#include <managers/SoundManager.hpp>
#include <managers/SettingsManager.hpp>

// Sounds
bool SoundManager::loadSound(std::filesystem::path const& filepath) {
    auto fullPath = (!filepath.is_absolute()) ? "assets" / filepath : filepath;

    if(!std::filesystem::exists(fullPath)) {
        return false;
    }

    auto key = fullPath.filename().string();
    auto sound = LoadSound(fullPath.string().c_str());
    m_sounds[key] = sound;

    return true;
}

void SoundManager::unloadSound(std::string const& key) {
    if(m_sounds.contains(key)) {
        UnloadSound(m_sounds[key]);
        m_sounds.erase(key);
    }
}

Sound& SoundManager::getSound(std::string const& key) {
    return m_sounds[key];
}

void SoundManager::setSoundVolume(float volume) {
    for(const auto& [key, value] : m_sounds) {
        SetSoundVolume(value, volume);
    }
}

// Tracks
bool SoundManager::loadMusic(std::filesystem::path const& filepath) {
    auto fullPath = (!filepath.is_absolute()) ? "assets" / filepath : filepath;

    if(!std::filesystem::exists(fullPath)) {
        return false;
    }

    auto key = fullPath.filename().string();
    auto track = LoadMusicStream(fullPath.string().c_str());
    m_tracks[key] = track;

    return true;
}

void SoundManager::unloadMusic(std::string const& key) {
    if(m_tracks.contains(key)) {
        UnloadMusicStream(m_tracks[key]);
        m_tracks.erase(key);
    }
}

Music& SoundManager::getMusic(std::string const& key) {
    return m_tracks[key];
}

void SoundManager::setMusicVolume(float volume) {
    for(const auto& [key, value] : m_tracks) {
        SetMusicVolume(value, volume);
    }
}