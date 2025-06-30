#include <raylib-cpp.hpp>
#include <unordered_map>
#include <string>
#include <filesystem>

class SoundManager {
public:
    static inline SoundManager& Get() {
        static SoundManager sm;
        return sm;
    }
    
    raylib::Sound& LoadSound(std::filesystem::path const& path);
    raylib::Music& LoadMusic(std::filesystem::path const& path);

    raylib::Sound& GetSound(std::string const& key);
    raylib::Music& GetMusic(std::string const& key);

private:
     std::unordered_map<std::string, raylib::Sound> m_sounds;
     std::unordered_map<std::string, raylib::Music> m_soundtracks;
};