#pragma once
#include <raylib-cpp.hpp>

namespace raylib {
    inline bool IsGestureDetected(unsigned int gesture) { return ::IsGestureDetected(gesture); }
    inline float GetGesturePinchAngle() { return ::GetGesturePinchAngle(); }

    class Scissor {
    public:
        explicit Scissor(::Rectangle rect) {
            ::BeginScissorMode(static_cast<int>(rect.x), static_cast<int>(rect.y),
                               static_cast<int>(rect.width), static_cast<int>(rect.height));
        }
        Scissor(const Scissor&) = delete;
        Scissor& operator=(const Scissor&) = delete;
        ~Scissor() { ::EndScissorMode(); }
    };
}
