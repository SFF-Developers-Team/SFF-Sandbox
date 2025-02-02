#include <Debug.hpp>
#include <raylib.h>
#include <GitHash.hpp>
#include <RenderManager.hpp>

Debug::Debug() {
    setString(GAME_VERSION, "SFF Sandbox {}-dev ({} {})", GitHash::shortSha1, __DATE__, __TIME__);
}

void Debug::draw() {
    //Какие же костыли...
    float y = -10.f;
    setString(FPS, "{} FPS", GetFPS());

    auto rm = RenderManager::get();

    for (auto& [id, str] : m_debugList) {
        rm->drawText("font", str, {0.f, y}, COL_WHITE, 40.f);
        y += 30.f;

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