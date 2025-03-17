#pragma once
#include <type_traits>
#include <cstdint>
#include <variant>
#include <cmath>
#include <memory>
#include <vector>
#include <map>

template <typename T>
struct Vec2 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Vec2");

    T x;
    T y;

    Vec2<T> const operator+(Vec2<T> other) { return Vec2<T> {x + other.x, y + other.y}; }
    Vec2<T> const operator+(T value) { return Vec2<T> {x + value, y + value}; }
    
    Vec2<T> const operator-(Vec2<T> other) { return Vec2<T> {x - other.x, y - other.y}; }
    Vec2<T> const operator-(T value) { return Vec2<T> {x - value, y - value}; }

    T length() { return std::sqrt((x * x) + (y * y)); }
    T lengthSqr() { return (x * x) + (y * y); }

    Vec2<T> lerp(Vec2<T> other, float a) { return {x + a * (other.x - x), y + a * (other.y - y)}; }

    template <typename T2>
    inline T2 to() const {
        return T2 {x, y};
    }

    template <typename T2>
    inline float distance(T2 other) const {
        return sqrtf((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }

    Vec2<T> const operator/(T value) { return Vec2<T> {x / value, y / value}; }
    bool operator==(Vec2<T> const& other) const { return x == other.x && y == other.y; }
};

template <typename T>
struct Vec2Hash {
    size_t operator()(Vec2<T> const& v) const { return std::hash<T>()(v.x) ^ (std::hash<T>()(v.y) << 1); }
};

template <typename T>
struct Vec3 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Vec3");

    T x;
    T y;
    T z;

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

    Rect<T> const anchor(Vec2<T> const& point) { return Rect<T> {x - width * point.x, y - height * point.y, width, height}; }

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

    bool operator==(Col3<T> const& other) const { return r == other.r && g == other.g && b == other.b; }

    template <typename T2>
    inline T2 to() const {
        return T2 {r, g, b};
    }

    void brightness(float factor) {
        float tr = static_cast<float>(r);
        float tg = static_cast<float>(g);
        float tb = static_cast<float>(b);

        if (factor > 0) {
            tr += (255.f - tr) * factor;
            tg += (255.f - tg) * factor;
            tb += (255.f - tb) * factor;
        } else {
            tr *= (1.0f + factor);
            tg *= (1.0f + factor);
            tb *= (1.0f + factor);
        }

        r = static_cast<uint8_t>(tr);
        g = static_cast<uint8_t>(tg);
        b = static_cast<uint8_t>(tb);
    }
};

template <typename T>
struct Col4 {
    static_assert(std::is_arithmetic_v<T>, "Unsupported type for Col4");

    T r;
    T g;
    T b;
    T a;

    Col4<T> operator+(const Col4<T>& other) { return {static_cast<T>(r + other.r), static_cast<T>(g + other.g), static_cast<T>(b + other.b), static_cast<T>(a + other.a)}; }
    Col4<T> operator-(const Col4<T>& other) { return {static_cast<T>(r - other.r), static_cast<T>(g - other.g), static_cast<T>(b - other.b), static_cast<T>(a - other.a)}; }
    Col4<T> operator*(const Col4<T>& other) { return {static_cast<T>(r * other.r), static_cast<T>(g * other.g), static_cast<T>(b * other.b), static_cast<T>(a * other.a)}; }
    Col4<T> operator/(const Col4<T>& other) { return {static_cast<T>(r / other.r), static_cast<T>(g / other.g), static_cast<T>(b / other.b), static_cast<T>(a / other.a)}; }
    Col4<T> operator*(T other) { return {static_cast<T>(r * other), static_cast<T>(g * other), static_cast<T>(b * other), static_cast<T>(a * other)}; }
    Col4<T> operator/(T other) { return {static_cast<T>(r / other), static_cast<T>(g / other), static_cast<T>(b / other), static_cast<T>(a / other)}; }

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

    // bruh i hate it
    void brightness(float factor) {
        float tr = static_cast<float>(r);
        float tg = static_cast<float>(g);
        float tb = static_cast<float>(b);

        if (factor > 0) {
            tr += (255.f - tr) * factor;
            tg += (255.f - tg) * factor;
            tb += (255.f - tb) * factor;
        } else {
            tr *= (1.0f + factor);
            tg *= (1.0f + factor);
            tb *= (1.0f + factor);
        }

        r = static_cast<uint8_t>(tr);
        g = static_cast<uint8_t>(tg);
        b = static_cast<uint8_t>(tb);
    }
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2fHash = Vec2Hash<float>;
using Vec2iHash = Vec2Hash<int>;

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

using Rectf = Rect<float>;
using Recti = Rect<int>;

using Col3u = Col3<unsigned char>;
using Col4u = Col4<unsigned char>;

// Custom raylib color palette for amazing visuals on WHITE background
#define COL_LIGHTGRAY  Col4u { 200, 200, 200, 255 }   // Light Gray
#define COL_GRAY       Col4u { 130, 130, 130, 255 }   // Gray
#define COL_DARKGRAY   Col4u { 80, 80, 80, 255 }      // Dark Gray
#define COL_YELLOW     Col4u { 253, 249, 0, 255 }     // Yellow
#define COL_GOLD       Col4u { 255, 203, 0, 255 }     // Gold
#define COL_ORANGE     Col4u { 255, 161, 0, 255 }     // Orange
#define COL_PINK       Col4u { 255, 109, 194, 255 }   // Pink
#define COL_RED        Col4u { 230, 41, 55, 255 }     // Red
#define COL_MAROON     Col4u { 190, 33, 55, 255 }     // Maroon
#define COL_GREEN      Col4u { 0, 228, 48, 255 }      // Green
#define COL_LIME       Col4u { 0, 158, 47, 255 }      // Lime
#define COL_DARKGREEN  Col4u { 0, 117, 44, 255 }      // Dark Green
#define COL_SKYBLUE    Col4u { 102, 191, 255, 255 }   // Sky Blue
#define COL_BLUE       Col4u { 0, 121, 241, 255 }     // Blue
#define COL_DARKBLUE   Col4u { 0, 82, 172, 255 }      // Dark Blue
#define COL_PURPLE     Col4u { 200, 122, 255, 255 }   // Purple
#define COL_VIOLET     Col4u { 135, 60, 190, 255 }    // Violet
#define COL_DARKPURPLE Col4u { 112, 31, 126, 255 }    // Dark Purple
#define COL_BEIGE      Col4u { 211, 176, 131, 255 }   // Beige
#define COL_BROWN      Col4u { 127, 106, 79, 255 }    // Brown
#define COL_DARKBROWN  Col4u { 76, 63, 47, 255 }      // Dark Brown
#define COL_WHITE      Col4u { 255, 255, 255, 255 }   // White
#define COL_BLACK      Col4u { 0, 0, 0, 255 }         // Black
#define COL_BLANK      Col4u { 0, 0, 0, 0 }           // Blank (Transparent)
#define COL_MAGENTA    Col4u { 255, 0, 255, 255 }     // Magenta
#define COL_RAYWHITE   Col4u { 245, 245, 245, 255 }   // My own White (raylib logo)

// Enums

enum DisconnectReasonID : uint32_t {
    INVALID_FIRST_PACKET,
    TOO_SHORT_USERNAME,
    TOO_LONG_USERNAME,
    USERNAME_ALREADY_TAKEN,
    KICKED_BY_SERVER
};

enum Direction : int8_t {
    RIGHT = -1,
    LEFT = 1
};

enum ControlType : uint8_t {
    CONTROL_KEYBOARD_MOUSE,
    CONTROL_GAMEPAD,
    CONTROL_TOUCH
};

enum GameMode : uint8_t {
    GAMEMODE_CREATIVE,
    GAMEMODE_SURVIVAL
};

enum ItemID : uint8_t {
    AIR = 0,
    GRASS,
    DIRT,
    STONE,
    COBBLESTONE,
    PLANKS,
    WOOL,
    BEDROCK,
    BRICKS,
    OAK_LOG,
    LEAVES,
    COAL_ORE,
    IRON_ORE_BLOCK,
    GOLD_ORE_BLOCK,
    DIAMOND_ORE,
    LAPIZ_ORE,
    BOOKSHELF,
    FLOWER_POT,
    FURNACE,
    ACTIVE_FURHANCE,
    STICK,
    WOODEN_PICKAXE,
    WOODEN_SWORD,
    STONE_PICKAXE,
    STONE_SWORD,
    IRON_PICKAXE,
    IRON_SWORD,
    GOLDEN_PICKAXE,
    GOLDEN_SWORD,
    DIAMOND_PICKAXE,
    DIAMOND_SWORD,
    IRON_ORE,
    GOLD_ORE,
    COAL,
    IRON_INGOT,
    GOLD_INGOT,
    DIAMOND
};

enum MaterialType : uint8_t {
    MATERIAL_NONE,
    MATERIAL_WOOD,
    MATERIAL_GOLD,
    MATERIAL_STONE,
    MATERIAL_IRON,
    MATERIAL_DIAMOND
};

enum ItemType : uint8_t {
    TYPE_BLOCK,
    TYPE_ITEM,
    TYPE_TOOL_PICKAXE,
    TYPE_TOOL_AXE,
    TYPE_TOOL_SHOVEL,
    TYPE_TOOL_HOE,
    TYPE_WEAPON
};

enum TagID : uint8_t {
    TAG_COLOR,
    TAG_GHOST,
    TAG_EFFICIENCY,
    TAG_DAMAGE,
    TAG_NATURAL
};

using TagValue = std::variant<Col3u, bool, uint8_t>;
using ItemTags = std::map<TagID, TagValue>;

struct BlockPosition {
    int x;
    int y;
    uint8_t layer = 1;

    bool operator==(BlockPosition const& other) const { return x == other.x && y == other.y && layer == other.layer; }
};

struct TreeStructure {
    int x;
    int y;
    int trunkHeight;
};

enum AnimationType : uint8_t {
    PLAYER_IDLE,
    PLAYER_MOVE,
    PLAYER_SNEAK,
    PLAYER_JUMP,
    PLAYER_HIT,
    PLAYER_HURT,
    PLAYER_SIT,
    PLAYER_CART
};

using ByteVector = std::vector<uint8_t>;

enum ObjectHeader : uint8_t {
    // World headers (also uses in mp)
    PLAYER,
    BLOCK,
    CHUNK,
    WORLD,
    ENTITY,
    ITEM,
    INVENTORY_ITEM,

    // Multiplayer headers
    IDENTIFICATION, DISCONNECT,
    LOAD_CHUNK,  UNLOAD_CHUNK,
    LOAD_PLAYER, UNLOAD_PLAYER,
    BLOCK_PLACE, BLOCK_DESTROY,

    NETWORK_ERROR, ARRAY, TERRAIN, LOAD_TERRAIN, LOAD_PLAYERS, MESSAGE, LOAD_MESSAGE,
    NULL_PACKET = 0xFF
};

enum WorldGenType : uint8_t {
    NORMAL
};

using PlayerID = uint32_t;