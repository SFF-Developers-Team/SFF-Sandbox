#pragma once
#include <Rectangle.hpp>

class Box : public raylib::Rectangle {
public:
    using raylib::Rectangle::Rectangle;

    float ClipCollideX(Box& other, float xa) const noexcept;
    float ClipCollideY(Box& other, float ya) const noexcept;

    void Move(float x, float y);

    Box Expand(float x, float y) const noexcept;
};