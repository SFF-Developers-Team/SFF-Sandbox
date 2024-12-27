#pragma once
#include <type_traits>

template <typename T>
struct Vec2 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Vec2");

    T x;
    T y;

    template <typename N>
    inline Vec2<N> toType() const {
        return {static_cast<N>(x), static_cast<N>(y)};
    }

    template <typename T2>
    inline T2 to() const {
        return T2 {x, y};
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

template <typename T>
struct Col3 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Col3");

    T r;
    T g;
    T b;

    Col3<T> operator+(const Col3<T>& other) { return {static_cast<T>(r + other.r), static_cast<T>(g + other.g), static_cast<T>(b + other.b)}; }
    Col3<T> operator-(const Col3<T>& other) { return {static_cast<T>(r - other.r), static_cast<T>(g - other.g), static_cast<T>(b - other.b)}; }
    Col3<T> operator*(const Col3<T>& other) { return {static_cast<T>(r * other.r), static_cast<T>(g * other.g), static_cast<T>(b * other.b)}; }
    Col3<T> operator/(const Col3<T>& other) { return {static_cast<T>(r / other.r), static_cast<T>(g / other.g), static_cast<T>(b / other.b)}; }
    Col3<T> operator*(T other) { return {static_cast<T>(r * other), static_cast<T>(g * other), static_cast<T>(b * other)}; }
    Col3<T> operator/(T other) { return {static_cast<T>(r / other), static_cast<T>(g / other), static_cast<T>(b / other)}; }
    void operator+=(const Col3<T>& other) { *this = *this + other; }
    void operator-=(const Col3<T>& other) { *this = *this - other; }
    void operator*=(const Col3<T>& other) { *this = *this * other; }
    void operator/=(const Col3<T>& other) { *this = *this / other; }
    void operator*=(T other) { *this = *this * other; }
    void operator/=(T other) { *this = *this / other; }

    template <typename T2>
    inline T2 to() const {
        return T2 {r, g, b};
    }
};

template <typename T>
struct Col4 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Col4");

    T r;
    T g;
    T b;
    T a;

    Col4<T> operator+(const Col4<T>& other) { return {r + other.r, g + other.g, b + other.b, a + other.a}; }
    Col4<T> operator-(const Col4<T>& other) { return {r - other.r, g - other.g, b - other.b, a - other.a}; }
    Col4<T> operator*(const Col4<T>& other) { return {r * other.r, g * other.g, b * other.b, a * other.a}; }
    Col4<T> operator/(const Col4<T>& other) { return {r / other.r, g / other.g, b / other.b, a / other.a}; }
    Col4<T> operator*(T other) { return {r * other, g * other, b * other, a * other}; }
    Col4<T> operator/(T other) { return {r / other, g / other, b / other, a / other}; }

    void operator+=(const Col4<T>& other) { *this = *this + other; }
    void operator-=(const Col4<T>& other) { *this = *this - other; }
    void operator*=(const Col4<T>& other) { *this = *this * other; }
    void operator/=(const Col4<T>& other) { *this = *this / other; }
    void operator*=(T other) { *this = *this * other; }
    void operator/=(T other) { *this = *this / other; }

    template <typename T2>
    inline T2 to() const {
        return T2 {r, g, b, a};
    }
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

using Rectf = Rect<float>;
using Recti = Rect<int>;

using Col3f = Col3<float>;
using Col3u = Col3<unsigned char>;

using Col4f = Col4<float>;
using Col4u = Col4<unsigned char>;