#pragma once
#include <string>
#include <vector>
#include <toml.hpp>

struct VideoMode {
    int width;
    int height;
};

class SettingsManager {
private:
    std::vector<VideoMode> m_modes;
    toml::table m_settings;

public:
    static SettingsManager* get() {
        static auto stm = new SettingsManager();
        return stm;
    }

    SettingsManager();
    template<typename T>
    void setValue(std::string const& path, T value, bool save = true) {
        toml::table* current = &m_settings;
        toml::path tomlPath(path);

        for (size_t i = 0; i < tomlPath.size() - 1; ++i) {
            std::string const& part = tomlPath[i].key();
            if (!current->contains(part)) {
                current->insert(part, toml::table{});
            }

            current = (*current)[part].as_table();
        }

        current->insert_or_assign(tomlPath[tomlPath.size() - 1].key(), value);
        
        if(save) {
            this->save();
        }
    }

    template<typename T>
    T getValue(std::string const& path, T defaultVal) {
        if(!m_settings.at_path(path)) {
            setValue(path, defaultVal);
        }

        return *m_settings.at_path(path).value<T>();
    }
    
    int getKeybind(std::string const& action);
    void setKeybind(std::string const& action, int key);

    /// @brief Creates keyboard.action if it doesn't exists
    void registerKeybind(std::string const& action, int key);

    void save();

    static std::string const getKeyName(int key);
    
    std::vector<std::string> const getKeyActions();
    std::vector<VideoMode> const& getModes();

    VideoMode getFullscreenSize();
};