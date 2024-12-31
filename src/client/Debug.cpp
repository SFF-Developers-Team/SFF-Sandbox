#include <Debug.hpp>
#include <raylib.h>

#define FONT_SIZE 20

void Debug::draw() {
    int y = 0;

    for (auto& str : m_debugList) {
        DrawText(str.c_str(), 0, y, FONT_SIZE, WHITE);
        y += FONT_SIZE + 2;
    }
}

void Debug::setVisible(bool v) {
    m_visible = v;
}

bool Debug::isVisible() {
    return m_visible;
}

void Debug::removeString(int index) {
    m_debugList.erase(m_debugList.begin() + index);
}