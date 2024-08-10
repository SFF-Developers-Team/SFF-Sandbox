#pragma once

template <typename T>
struct Vec2 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Vec2");

    T x;
    T y;

    template <typename N>
    inline Vec2<N> toType() const {
        return {static_cast<N>(x), static_cast<N>(y)};
    }
};

template <typename T>
struct Rect {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Rect");

    T x;
    T y;
    T width;
    T height;

    bool doesIntersect(const Rect<T>& other) const { return x < other.x + other.width && x + width > other.x && y > other.y + other.height && y + height < other.y; }

    bool contains(const Vec2<T>& other) const { return other.x >= x && other.x <= x + width && other.y >= y && other.y <= y + height; }

    template <typename T2>
    inline Rect<T2> toType() const {
        return {static_cast<T2>(x), static_cast<T2>(y), static_cast<T2>(width), static_cast<T2>(height)};
    }

    template <typename T2>
    inline T2 to() const {
        return T2 {x, y, width, height};
    }
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

using Rectf = Rect<float>;
using Recti = Rect<int>;
