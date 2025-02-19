#include <SettingsManager.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <Logger.hpp>
#include <Game.hpp>
#include <SoundManager.hpp>
#include <glfw3.h>

std::map<std::string, int> keybinds = {
    {"jump", KEY_W},
    {"duck", KEY_S},
    {"left", KEY_A},
    {"right", KEY_D},
    {"fly", KEY_F}
};

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

    try {
        m_settings = toml::parse_file("settings.toml");
    } catch(toml::parse_error const& e) {
        logE("Failed to init settings! {}", e.description());
        m_settings = toml::table{};
    }

    for(auto const& [action, key] : keybinds) {
        registerKeybind(action, key);
    }

    auto game = Game::get();
    auto audio = SoundManager::get();
    audio->setMusicVolume(getValue<float>("audio.volume.music", 0.5f));
    audio->setSoundVolume(getValue<float>("audio.volume.sound", 0.5f));
    SetMasterVolume(getValue<float>("audio.volume.general", 1.f));
    game->setGuiScale(getValue<int>("video.scale", 1));
    (getValue<bool>("video.vsync", true) ? SetWindowState(FLAG_VSYNC_HINT) : ClearWindowState(FLAG_VSYNC_HINT));
}

int SettingsManager::getKeybind(std::string const& action) {
    return getValue<int>("keyboard." + action, 0);
}
void SettingsManager::setKeybind(std::string const& action, int key) {
    setValue("keyboard." + action, key);
}

void SettingsManager::registerKeybind(std::string const& action, int key) {
    if(!m_settings.contains("keyboard")) {
        m_settings.insert("keyboard", toml::table{});
    }

    auto keyboard = m_settings["keyboard"].as_table();

    if(!keyboard->contains(action)) {
        keyboard->insert(action, key);
    }
}

void SettingsManager::save() {
    std::ofstream file("settings.toml");
    if(file.is_open()) {
        file << m_settings;
        return file.close();
    }

    logE("Failed to save settings!");
}

std::vector<std::string> const SettingsManager::getKeyActions() {
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

std::string const SettingsManager::getKeyName(int key) {
    if (key >= 39 && key <= 125) return std::string((char*)&key);
    
    switch (key) {
        case KEY_SPACE: return "Space";
        case KEY_ESCAPE: return "Escape";
        case KEY_ENTER: return "Enter";
        case KEY_TAB: return "Tab";
        case KEY_BACKSPACE: return "Backspace";
        case KEY_INSERT: return "Insert";
        case KEY_DELETE: return "Delete";
        case KEY_RIGHT: return "Right";
        case KEY_LEFT: return "Left";
        case KEY_DOWN: return "Down";
        case KEY_UP: return "Up";
        case KEY_PAGE_UP: return "Page Up";
        case KEY_PAGE_DOWN: return "Page Down";
        case KEY_HOME: return "Home";
        case KEY_END: return "End";
        case KEY_CAPS_LOCK: return "Caps Lock";
        case KEY_SCROLL_LOCK: return "Scroll Lock";
        case KEY_NUM_LOCK: return "Num Lock";
        case KEY_PRINT_SCREEN: return "Print Screen";
        case KEY_PAUSE: return "Pause";
        case KEY_F1: return "F1";
        case KEY_F2: return "F2";
        case KEY_F3: return "F3";
        case KEY_F4: return "F4";
        case KEY_F5: return "F5";
        case KEY_F6: return "F6";
        case KEY_F7: return "F7";
        case KEY_F8: return "F8";
        case KEY_F9: return "F9";
        case KEY_F10: return "F10";
        case KEY_F11: return "F11";
        case KEY_F12: return "F12";
        case KEY_LEFT_SHIFT: return "Left Shift";
        case KEY_LEFT_CONTROL: return "Left Ctrl";
        case KEY_LEFT_ALT: return "Left Alt";
        case KEY_LEFT_SUPER: return "Left Super";
        case KEY_RIGHT_SHIFT: return "Right Shift";
        case KEY_RIGHT_CONTROL: return "Right Ctrl";
        case KEY_RIGHT_ALT: return "Right Alt";
        case KEY_RIGHT_SUPER: return "Right Super";
        case KEY_KB_MENU: return "KB Menu";
        case KEY_KP_0: return "KP 0";
        case KEY_KP_1: return "KP 1";
        case KEY_KP_2: return "KP 2";
        case KEY_KP_3: return "KP 3";
        case KEY_KP_4: return "KP 4";
        case KEY_KP_5: return "KP 5";
        case KEY_KP_6: return "KP 6";
        case KEY_KP_7: return "KP 7";
        case KEY_KP_8: return "KP 8";
        case KEY_KP_9: return "KP 9";
        case KEY_KP_DECIMAL: return "KP .";
        case KEY_KP_DIVIDE: return "KP /";
        case KEY_KP_MULTIPLY: return "KP *";
        case KEY_KP_SUBTRACT: return "KP -";
        case KEY_KP_ADD: return "KP +";
        case KEY_KP_ENTER: return "KP Enter";
        case KEY_KP_EQUAL: return "KP =";
        default: return "Unknown";
    }
}