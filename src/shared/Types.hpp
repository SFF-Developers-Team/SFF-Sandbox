#pragma once
#include <type_traits>
#include <cstdint>
#include <cmath>
#include <memory>
#include <vector>

#ifdef GOLD
#   undef GOLD
#endif

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

    template <typename U>
    inline Vec2<U> as() const { return Vec2<U> {static_cast<U>(x), static_cast<U>(y)}; }

    template <typename U>
    inline U to() const { return U {x, y}; }

    template <typename T2>
    inline float distance(T2 other) const {
        return sqrtf((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }

    Vec2<T> const operator/(T value) { return Vec2<T> {x / value, y / value}; }
    bool operator==(Vec2<T> const& other) const { return x == other.x && y == other.y; }
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

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

using Rectf = Rect<float>;
using Recti = Rect<int>;

// Enums

enum class DisconnectReason : uint32_t {
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

#define ITEM_ID_BEGIN 7000

enum class TileID : uint16_t {
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
    LEAVES = 11,
    BOOKSHELF,
    FLOWER_POT,
    FURNACE,
    ACTIVE_FURHANCE,
    WORKBENCH,
    CHEST,
    BED,
    DEAD_ROSE = BED + 2,
    ROSE,
    IRON_ORE,
    COAL_ORE,
    DIAMOND_ORE,
    GOLD_ORE,
    TORCH,
    GLASS = TORCH + 8,
    SIGN,
    SNOW,
    SNOW_CARPET,
    SAND,
    CACTUS_HEAD,
    CACTUS_BASE,
    TALL_GRASS,
    MEDIUM_GRASS,
    TINY_GRASS
};

enum class ItemID : uint16_t {
    // blocks.png
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
    BOOKSHELF,
    FLOWER_POT,
    FURNACE,
    WORKBENCH,
    CHEST,
    BED,
    DEAD_ROSE,
    ROSE,
    GLASS,
    SIGN,
    SNOW,
    SAND,

    // items.png
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
    DIAMOND,
    WOODEN_AXE,
    STONE_AXE,
    IRON_AXE,
    GOLDEN_AXE,
    DIAMOND_AXE,
    WOODEN_SHOVEL,
    STONE_SHOVEL,
    IRON_SHOVEL,
    GOLDEN_SHOVEL,
    DIAMOND_SHOVEL,
    WOODEN_HOE,
    STONE_HOE,
    IRON_HOE,
    GOLDEN_HOE,
    DIAMOND_HOE,
    TORCH,
    FISHING_ROPE,
    FULL_BUCKET,
    EMPTY_BUCKET,
    BOOK,
    BOW,
    ARROW,
    FEATHER,
    APPLE,
    STEAK,
    BONE,
    EGG
};

enum class ItemMaterial : uint8_t {NONE, WOOD, GOLD, STONE, IRON, DIAMOND};

enum ItemType : uint8_t {
    TYPE_TILE,
    TYPE_ITEM,
    TYPE_TOOL_PICKAXE,
    TYPE_TOOL_AXE,
    TYPE_TOOL_SHOVEL,
    TYPE_TOOL_HOE,
    TYPE_WEAPON
};

struct TilePosition {
    int x;
    int y;
    uint8_t layer = 1;

    bool operator==(TilePosition const& other) const { return x == other.x && y == other.y && layer == other.layer; }
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
    TILE,
    CHUNK,
    WORLD,
    ENTITY,
    ITEM,
    INVENTORY_ITEM,

    // Multiplayer headers
    IDENTIFICATION, DISCONNECT,
    LOAD_CHUNK,  UNLOAD_CHUNK,
    LOAD_PLAYER, UNLOAD_PLAYER,
    TILE_PLACE, TILE_DESTROY,

    NETWORK_ERROR, ARRAY, TERRAIN, LOAD_TERRAIN, LOAD_PLAYERS, MESSAGE, LOAD_MESSAGE,
    NULL_PACKET = 0xFF
};

enum class WorldGenType : uint8_t { NORMAL, FLAT, CAVES };

using PlayerID = uint32_t;
using TileVector = std::vector<std::shared_ptr<class Tile>>;