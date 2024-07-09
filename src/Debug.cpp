#include "Debug.hpp"
#include <raylib.h>

#define FONT_SIZE 20

void Debug::addString(std::string str) {
    if(m_debug) {
        m_debugList.push_back(str);
    }
}

void Debug::draw() {
    int y = 0;
    for (auto& str : m_debugList) {
        DrawText(str.c_str(), 0, y, FONT_SIZE, WHITE);
        y += FONT_SIZE + 2;
    }

    m_debugList.clear();
}