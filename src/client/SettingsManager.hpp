#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include <rlgl.h>
#include <map>
#include <glfw3.h>
enum KeyID {
    JUMP,
    UP,
    DOWN,
    LEFT_K,
    RIGHT_K,
    FLY,
    RESET_POSITION,
};
class SettingsManager {

private:
    bool m_selectMode = false;

    std::map<KeyID, int> m_bindings = {
        {JUMP, KEY_SPACE},
        {UP, KEY_W},
        {DOWN, KEY_S},
        {LEFT_K, KEY_A},
        {RIGHT_K, KEY_D},
        {FLY, KEY_F},
        {RESET_POSITION, KEY_R},
    };
    int modeCount;
    const GLFWvidmode* modes;
public:
    static SettingsManager* get() {
        static auto stm = new SettingsManager();
        return stm;
    }

    SettingsManager();
    std::string const getKeyNameFromID(KeyID id);
    int getKeyFromID(KeyID);
    std::map<KeyID, int>& getBindings();
    void setSelectKey(KeyID id, int key);    
    std::vector<GLFWvidmode> getModes();
};