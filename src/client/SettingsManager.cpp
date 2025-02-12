#include <SettingsManager.hpp>
#include <filesystem>
#include <fstream>
#include <Logger.hpp>

SettingsManager::SettingsManager() {
    // shitcode
    auto monitorI = GetCurrentMonitor();
    auto monitor = glfwGetPrimaryMonitor();
    auto modeCount = 0;

    auto modes = glfwGetVideoModes(monitor, &modeCount);
    for(auto i = 0; i < modeCount; i++) {
        if(modes[i].width <= GetMonitorWidth(monitorI) && modes[i].height <= GetMonitorHeight(monitorI)) {
            auto it = std::find_if(m_modes.begin(), m_modes.end(), [&](auto const& mode) { 
                return modes[i].width == mode.width && modes[i].height == mode.height; 
            });

            if(it == m_modes.end()) {
                m_modes.push_back(VideoMode {modes[i].width, modes[i].height});
            }
        }
    }

    // m_settings = toml::table {
    //     {"audio", toml::table {
    //         {"volume", 0.5f},
    //         {"music", 0.5f},
    //         {"sound", 0.5f}
    //     }},
    //     {"video", toml::table {
    //         {"fullscreen", false},
    //         {"resolution", 0}
    //     }},
    //     {"keyboard", toml::table {
    //         {"jump", KEY_SPACE},
    //         {"duck", KEY_LEFT_SHIFT},
    //         {"left", KEY_A},
    //         {"right", KEY_D},
    //         {"fly", KEY_F}
    //     }}
    // };

    try {
        m_settings = toml::parse_file("settings.toml");
    } catch(toml::parse_error const& e) {
        logE("Failed to init settings! {}", e.description());
    }
}

int SettingsManager::getKeybind(std::string const& action) {
    return getValue<int>("keyboard." + action, 0);
}

void SettingsManager::setKeybind(std::string const& action, int key) {
    setValue("keyboard." + action, key);
}

std::vector<std::string> const SettingsManager::getKeyList() {
    auto table = m_settings["keyboard"].as_table();
    std::vector<std::string> ret;
    
    for(auto const& [action, key] : *table) {
        ret.push_back(action.str().data());
    }

    return ret;
}

std::vector<VideoMode> const& SettingsManager::getModes() {
    return m_modes;
}