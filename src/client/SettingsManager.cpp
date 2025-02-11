#include <SettingsManager.hpp>
#include <filesystem>
#include <fstream>
#include <Logger.hpp>

std::string_view const defaultConfig = R"([Audio]
volume=0.5
music=0.5
sound=0.5

[Video]
fullscreen=true
resolution=0

[Keyboard]
Jump=31
Duck=340
Left=65
Right=68
Fly=70)";

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

    std::ifstream file("settings.toml");
    std::string settings;

    if(file.is_open()) {
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        settings.resize(size);
        file.read(settings.data(), size);
    } else {
        logE("Failed to init settings!");
        settings = defaultConfig;
    }

    m_settings = toml::parse(settings);

}

int SettingsManager::getKeybind(std::string const& action) {
    return getValue<int>("Keyboard/" + action, 0);
}

void SettingsManager::setKeybind(std::string const& action, int key) {
    setValue("Keyboard/" + action, key);
}

std::vector<std::string> const& SettingsManager::getKeyList() {
    return m_keylist;
}

std::vector<VideoMode> const& SettingsManager::getModes() {
    return m_modes;
}