#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include <rlgl.h>
#include <map>
#include <glfw3.h>
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
    void setValue(std::string const& path, T value) {
        auto tpath = toml::path(path);
        auto lastdot = path.find_last_of('.');

        if(lastdot != std::string::npos) {
            auto last = path.substr(lastdot + 1);
            auto table = m_settings[tpath.parent()].as_table();

            table->insert_or_assign(last, value);
        } else {
            m_settings.insert_or_assign(path, value);
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
    void registerKeybind(std::string const& action);
    
    std::vector<std::string> const getKeyList();
    std::vector<VideoMode> const& getModes();
};