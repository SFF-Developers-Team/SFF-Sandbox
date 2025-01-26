#include <SettingsManager.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <Logger.hpp>

std::string_view const defaultConfig = R"('Video/fullscreen' = false
'Video/vsync' = 1
'Video/resolution' = 0
'Video/guiscale' = 1

'Audio/volume' = 0.5
'Audio/music' = 0.5
'Audio/sound' = 0.5

'Keyboard/Duck' = 340
'Keyboard/Fly' = 70
'Keyboard/Jump' = 32
'Keyboard/Left' = 65
'Keyboard/Right' = 68)";

SettingsManager::SettingsManager() {
    auto monitor = 	glfwGetPrimaryMonitor();
    auto modeCount = 0;
    auto modes = glfwGetVideoModes(monitor, &modeCount);
    m_modes = std::vector(modes, modes +  modeCount);
    
    std::fstream file("settings.toml", std::ios::out | std::ios::in);
    std::string settings;
    std::string strConf;
    strConf = defaultConfig;

    if(file.is_open()) {
        logD("Opened");
        std::stringstream buffer;
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);
        settings.resize(size);
        buffer << file.rdbuf();
        settings = buffer.str();
    } else {    
        logE("Failed to init settings!");
        settings = defaultConfig;
    }
    logD("{}", settings);
    m_settings = toml::parse(settings);
    m_defconf = toml::parse(strConf);
    
    logD("{}", m_settings.size());
}
void SettingsManager::save() {
    std::ofstream file("settings.toml");
    logD("{}", m_settings.size());
    if(file.is_open()) {
        file << m_settings;
        logD("Successfully written");
    } else {
        logE("Failed to init config!");
    }
}
int SettingsManager::getKeybind(std::string const& action) {
    return getValue<int>("Keyboard/" + action, *m_defconf[action].value<int>());
}
std::string SettingsManager::getKeyName(int key) {
    if (key >= 39 && key <= 125) return std::to_string((char)key);
    switch (key) {
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
void SettingsManager::setKeybind(std::string const& action, int key) {
    setValue("Keyboard/" + action, key);
}

std::vector<std::string> const& SettingsManager::getKeyList() {
    return m_keylist;
}

std::vector<GLFWvidmode> const& SettingsManager::getModes() {
    return m_modes;
}