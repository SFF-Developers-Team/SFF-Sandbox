#pragma once
#include <Item.hpp>
#include <Types.hpp>
#include <unordered_map>
#include <optional>

class World;

class Tile {
private:
    static bool m_inited;
    static std::unordered_map<TileID, std::unique_ptr<Tile>> m_tileObjectDb;

public:
    Tile() = default;
    Tile(const Tile& tile) = delete;
    virtual ~Tile() = default;

    static void init();
    static Tile* Get(TileID id);

    static float GetDurability(TileID id);
    static float GetBreakingSpeed(TileID id, Item* tool = nullptr);
    static bool IsPhantom(TileID id);
    static bool IsTransparent(TileID id);
    static std::string_view GetName(TileID id);
    static std::optional<ItemID> GetDrop(TileID id, Item* tool = nullptr);

    virtual void onTick(World& world, TilePosition position) {}
    virtual void onRandomTick(World& world, TilePosition position) {}
    virtual void tileChanged(World& world, TilePosition position) {}
};