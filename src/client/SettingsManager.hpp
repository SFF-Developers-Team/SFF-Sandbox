#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include <rlgl.h>
#include <map>
#include <glfw3.h>
#include <toml.hpp>

class SettingsManager {
private:
    std::vector<GLFWvidmode> m_modes;
    toml::table m_settings;
    toml::table m_defconf;
    std::vector<std::string> const m_keylist = {"Jump", "Duck", "Left", "Right", "Fly"};

public:
    static SettingsManager* get() {
        static auto stm = new SettingsManager();
        return stm;
    }

    SettingsManager();
    template<typename T>
    void setValue(std::string const& path, T value) {
        m_settings.insert_or_assign(path, value);
    }

    template<typename T>
    T getValue(std::string const& path, T defaultVal) {
        return m_settings.at_path(path).value_or(defaultVal);
    }
    std::string getKeyName(int key);
    int getKeybind(std::string const& action);
    void setKeybind(std::string const& action, int key);
    
    std::vector<std::string> const& getKeyList();
    std::vector<GLFWvidmode> const& getModes();

    void save();
};