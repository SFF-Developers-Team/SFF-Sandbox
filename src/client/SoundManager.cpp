#include <SoundManager.hpp>
// Sounds
bool SoundManager::loadSound(std::filesystem::path const& filepath) {
    if(!std::filesystem::exists(filepath)) {
        return false;
    }

    auto key = filepath.filename().string();
    auto sound = LoadSound(filepath.string().c_str());
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
    if(!std::filesystem::exists(filepath)) {
        return false;
    }

    auto key = filepath.filename().string();
    auto track = LoadMusicStream(filepath.string().c_str());
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