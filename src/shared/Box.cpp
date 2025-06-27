#include "Box.hpp"

float Box::ClipCollideX(Box& other, float xa) const noexcept {
    if (other.y + other.height <= y || other.y >= y + height) {
        return xa;
    }

    if (xa > 0.0f && other.x + other.width <= x)
        xa = std::min(x - (other.x + other.width), xa);

    if (xa < 0.0f && other.x >= x + width)
        xa = std::max(xa, (x + width) - other.x);

    return xa;
}

float Box::ClipCollideY(Box& other, float ya) const noexcept {
    if (other.x + other.width <= x || other.x >= x + width) {
        return ya;
    }

    if (ya > 0.0f && other.y + other.height <= y)
        ya = std::min(ya, y - (other.y + other.height));

    if (ya < 0.0f && other.y >= y + height)
        ya = std::max(ya, (y + height) - other.y);

    return ya;
}

void Box::Move(float x, float y) {
    this->x += x;
    this->y += y;
}

Box Box::Expand(float x, float y) const noexcept {
    Box expanded = *this;

    expanded.x += (x < 0.f) ? x : 0.f;
    expanded.width += (x > 0.f) ? x : std::abs(x);

    expanded.y += (y < 0.f) ? y : 0.f;
    expanded.height += (y > 0.f) ? y : std::abs(y);

    return expanded;
}