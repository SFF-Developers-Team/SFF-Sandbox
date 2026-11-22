#pragma once
#include <Item.hpp>
#include <Types.hpp>
#include <string_view>

class World;

class Tile {
private:
    static bool m_inited;
    static std::unordered_map<TileID, std::unique_ptr<Tile>> m_tilesDb;

public:
    Tile() = default;
    Tile(Tile& tile) = delete;

    virtual ~Tile() = default;

    static void init();
    static Tile* get(TileID id);
    static float getDurability(TileID id);
    static std::string_view getName(TileID id);

    virtual TileID getId() const = 0;

    virtual void onTick(World& world, TilePosition position) {}
    virtual void onRandomTick(World& world, TilePosition position) {}

    virtual std::unique_ptr<Item> dropItem(Item* tool) {
        return std::make_unique<Item>(getId());
    }
};