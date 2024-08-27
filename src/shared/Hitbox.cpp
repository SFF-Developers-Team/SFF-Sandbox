#include <Hitbox.hpp>
#include <Logger.hpp>

Hitbox::Hitbox(Rectf rec) : x(rec.x), y(rec.y), width(rec.width), height(rec.height) {}

Hitbox::Hitbox(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {} 

float Hitbox::clipXCollide(Hitbox& other, float xa) {
    if (other.y + other.height <= y || other.y >= y + height) {
        return xa;
    }

    if (xa > 0.0f && other.x + other.width <= x)
        xa = std::min(x - (other.x + other.width), xa);

    if (xa < 0.0f && other.x >= x + width)
        xa = std::max(xa, (x + width) - other.x);

    return xa;
}

float Hitbox::clipYCollide(Hitbox& other, float ya) {
    if (other.x + other.width <= x || other.x >= x + width) {
        return ya;
    }

    if (ya > 0.0f && other.y + other.height <= y)
        ya = std::min(ya, y - (other.y + other.height));

    if (ya < 0.0f && other.y >= y + height)
        ya = std::max(ya, (y + height) - other.y);

    return ya;
}

bool Hitbox::intersects(Hitbox& otherHitbox) {
    // Check on X axis
    if (otherHitbox.x + otherHitbox.width <= x || otherHitbox.x >= x + width) {
        return false;
    }

    // Check on Y axis
    if (otherHitbox.y + otherHitbox.height <= y || otherHitbox.y >= y + height) {
        return false;
    }

    return true;
}

void Hitbox::move(float x, float y) {
    this->x += x;
    this->y += y;
}

Hitbox Hitbox::expand(float x, float y) {
    Rectf newh = getRect();

    if (x < 0.f) {
        newh.x += x;
        newh.width += std::abs(x);
    }

    if (x > 0.f) {
        newh.width += x;
    }

    if (y < 0.f) {
        newh.y += y;
        newh.height += std::abs(y);
    }

    if (y > 0.f) {
        newh.height += y;
    }

    return Hitbox(newh);
}