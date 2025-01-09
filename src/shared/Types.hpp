#pragma once
#include <type_traits>
#include <stdint.h>
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

    template <typename T2>
    inline float distance(T2 other) const {
        return sqrtf((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};

template <typename T>
struct Vec3 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Vec3");

    T x;
    T y;
    T z;

    template <typename N>
    inline Vec2<N> toType() const {
        return {static_cast<N>(x), static_cast<N>(y), static_cast<N>(z)};
    }

    template <typename T2>
    inline T2 to() const {
        return T2 {x, y, z};
    }

    bool operator==(Vec3 const& other) { return (x == other.x && y == other.y && z == other.z); }
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

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

using Rectf = Rect<float>;
using Recti = Rect<int>;

using Col3f = Col3<float>;
using Col3u = Col3<unsigned char>;

using Col4f = Col4<float>;
using Col4u = Col4<unsigned char>;

// Custom raylib color palette for amazing visuals on WHITE background
#define COL_LIGHTGRAY  (Col4u){ 200, 200, 200, 255 }   // Light Gray
#define COL_GRAY       (Col4u){ 130, 130, 130, 255 }   // Gray
#define COL_DARKGRAY   (Col4u){ 80, 80, 80, 255 }      // Dark Gray
#define COL_YELLOW     (Col4u){ 253, 249, 0, 255 }     // Yellow
#define COL_GOLD       (Col4u){ 255, 203, 0, 255 }     // Gold
#define COL_ORANGE     (Col4u){ 255, 161, 0, 255 }     // Orange
#define COL_PINK       (Col4u){ 255, 109, 194, 255 }   // Pink
#define COL_RED        (Col4u){ 230, 41, 55, 255 }     // Red
#define COL_MAROON     (Col4u){ 190, 33, 55, 255 }     // Maroon
#define COL_GREEN      (Col4u){ 0, 228, 48, 255 }      // Green
#define COL_LIME       (Col4u){ 0, 158, 47, 255 }      // Lime
#define COL_DARKGREEN  (Col4u){ 0, 117, 44, 255 }      // Dark Green
#define COL_SKYBLUE    (Col4u){ 102, 191, 255, 255 }   // Sky Blue
#define COL_BLUE       (Col4u){ 0, 121, 241, 255 }     // Blue
#define COL_DARKBLUE   (Col4u){ 0, 82, 172, 255 }      // Dark Blue
#define COL_PURPLE     (Col4u){ 200, 122, 255, 255 }   // Purple
#define COL_VIOLET     (Col4u){ 135, 60, 190, 255 }    // Violet
#define COL_DARKPURPLE (Col4u){ 112, 31, 126, 255 }    // Dark Purple
#define COL_BEIGE      (Col4u){ 211, 176, 131, 255 }   // Beige
#define COL_BROWN      (Col4u){ 127, 106, 79, 255 }    // Brown
#define COL_DARKBROWN  (Col4u){ 76, 63, 47, 255 }      // Dark Brown
#define COL_WHITE      (Col4u){ 255, 255, 255, 255 }   // White
#define COL_BLACK      (Col4u){ 0, 0, 0, 255 }         // Black
#define COL_BLANK      (Col4u){ 0, 0, 0, 0 }           // Blank (Transparent)
#define COL_MAGENTA    (Col4u){ 255, 0, 255, 255 }     // Magenta
#define COL_RAYWHITE   (Col4u){ 245, 245, 245, 255 }   // My own White (raylib logo)

enum DisconnectReasonID : uint32_t {
    INVALID_FIRST_PACKET,
    TOO_SHORT_USERNAME,
    TOO_LONG_USERNAME,
    USERNAME_ALREADY_TAKEN
};

enum Direction : uint8_t {
    LEFT = 0,
    RIGHT = 1
};