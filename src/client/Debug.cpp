#include <Debug.hpp>
#include <raylib.h>
#include <GitHash.hpp>

#define FONT_SIZE 20

Debug::Debug() {
    setString(GAME_VERSION, "SFF Sandbox {}-dev ({} {})", GitHash::shortSha1, __DATE__, __TIME__);
}

void Debug::draw() {
    int y = 0;
    setString(FPS, "{} FPS", GetFPS());

    for (auto& [id, str] : m_debugList) {
        DrawText(str.c_str(), 0, y, FONT_SIZE, WHITE);
        y += FONT_SIZE + 2;

        if(!isVisible() && (id != GAME_VERSION || id != FPS)) {
            break;
        }
    }
}

void Debug::setVisible(bool v) {
    m_visible = v;
}

bool Debug::isVisible() {
    return m_visible;
}

void Debug::removeString(DebugID id) {
    m_debugList.erase(id);
}