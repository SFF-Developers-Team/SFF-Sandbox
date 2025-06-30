#include "Debug.hpp"
#include <raylib-cpp.hpp>

void Debug::Draw() {
    for (auto& info : m_info) {
        raylib::DrawText(info, 0, 0, 20, RAYWHITE);
    }
}