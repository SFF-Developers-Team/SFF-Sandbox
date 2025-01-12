#include <SettingsManager.hpp>
SettingsManager::SettingsManager() {
    auto monitor = 	glfwGetPrimaryMonitor();
    modes = glfwGetVideoModes(monitor, &modeCount);
}
std::string const SettingsManager::getKeyNameFromID(KeyID id) {
    switch(id) {
        case JUMP: return "Jump";
        case UP: return "Up";
        case DOWN: return "Down";
        case LEFT_K: return "Left";
        case RIGHT_K: return "Right";
        case FLY: return "Fly";
        case RESET_POSITION: return "Reset Position";
        default: return "Unknown";
    }
}
std::map<KeyID, int>& SettingsManager::getBindings() {
    return m_bindings;
}
void SettingsManager::setSelectKey(KeyID id, int key) {
    m_bindings[id] = key; 
}   
int SettingsManager::getKeyFromID(KeyID id) {
    return m_bindings[id];
}
std::vector<GLFWvidmode> SettingsManager::getModes() {
    std::vector<GLFWvidmode> modeList(modes, modes + modeCount);
    return modeList;
}
