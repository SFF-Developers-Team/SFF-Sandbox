#include <Debug.hpp>
#include <raylib.h>
#include <GitHash.hpp>
#include <RenderManager.hpp>

Debug::Debug() {
    setString(GAME_VERSION, "SFF Sandbox {}-dev ({} {})", GitHash::shortSha1, __DATE__, __TIME__);
}

void Debug::draw() {
    float y = 0.f;
    setString(FPS, "{} FPS", GetFPS());

    for (auto& [id, str] : m_debugList) {
        auto size = RenderManager::getTextSize(str, "font", 20.f);
        RenderManager::drawRect({0.f, y, size.x, size.y}, {0, 0, 0, 128});
        RenderManager::drawText("font", str, {0.f, y}, COL_WHITE, 20.f);
        y += size.y;

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