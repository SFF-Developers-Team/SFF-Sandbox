#pragma once
#include <filesystem>
#include <raylib.h>
#include <string>
#include <map>

class SoundManager {
private:
    std::map<std::string, Sound> m_sounds;
    std::map<std::string, Music> m_tracks;

public:
    static auto get() {
        static auto sm = std::make_shared<SoundManager>();
        return sm;
    }
    bool loadSound(std::filesystem::path const& filepath);
    void unloadSound(std::string const& key);
    Sound& getSound(std::string const& key);

    bool loadMusic(std::filesystem::path const& filepath);
    void unloadMusic(std::string const& key);
    Music& getMusic(std::string const& key);
};