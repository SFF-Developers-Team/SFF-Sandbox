#include "Tile.hpp"
#include "tiles/CoalOre.hpp"
#include "tiles/DiamondOre.hpp"
#include "tiles/GoldOre.hpp"
#include "tiles/Grass.hpp"
#include "tiles/IronOre.hpp"
#include "tiles/Leaves.hpp"
#include "tiles/Stone.hpp"

struct TileProperties {
    std::string_view name;
    ItemType type;
    float durability;
    ItemType toolType;
};

bool Tile::m_inited = false;
std::unordered_map<TileID, std::unique_ptr<Tile>> Tile::m_tilesDb;

//   id                   name            type        durability  best tool
std::unordered_map<TileID, TileProperties> TilesDB = {
    {TileID::GRASS,       {"Grass",       TYPE_TILE,  0.6f,       TYPE_TOOL_SHOVEL}},
    {TileID::DIRT,        {"Dirt",        TYPE_TILE,  0.5f,       TYPE_TOOL_SHOVEL}},
    {TileID::STONE,       {"Stone",       TYPE_TILE,  1.5f,       TYPE_TOOL_PICKAXE}},
    {TileID::COBBLESTONE, {"Cobblestone", TYPE_TILE,  2.0f,       TYPE_TOOL_PICKAXE}},
    {TileID::PLANKS,      {"Planks",      TYPE_TILE,  2.0f,       TYPE_TOOL_AXE}},
    {TileID::WOOL,        {"Wool",        TYPE_TILE,  0.8f,       TYPE_ITEM}},
    {TileID::BEDROCK,     {"Bedrock",     TYPE_TILE, -1.0f,       TYPE_ITEM}},
    {TileID::BRICKS,      {"Bricks",      TYPE_TILE,  2.0f,       TYPE_TOOL_PICKAXE}},
    {TileID::OAK_LOG,     {"Oak log",     TYPE_TILE,  2.0f,       TYPE_TOOL_AXE}},
    {TileID::LEAVES,      {"Leaves",      TYPE_TILE,  0.2f,       TYPE_ITEM}},
    {TileID::COAL_ORE,    {"Coal ore",    TYPE_TILE,  3.0f,       TYPE_TOOL_PICKAXE}},
    {TileID::IRON_ORE,    {"Iron ore",    TYPE_TILE,  3.0f,       TYPE_TOOL_PICKAXE}},
    {TileID::GOLD_ORE,    {"Gold ore",    TYPE_TILE,  3.0f,       TYPE_TOOL_PICKAXE}},
    {TileID::DIAMOND_ORE, {"Diamond ore", TYPE_TILE,  3.0f,       TYPE_TOOL_PICKAXE}},
    {TileID::LAPIZ_ORE,   {"Lapiz ore",   TYPE_TILE,  3.0f,       TYPE_TOOL_PICKAXE}},
    {TileID::BOOKSHELF,   {"Bookshelf",   TYPE_TILE,  2.0f,       TYPE_TOOL_AXE}},
    {TileID::FLOWER_POT,  {"Flower pot",  TYPE_TILE,  0.0f,       TYPE_ITEM}},
    {TileID::FURNACE,     {"Furnace",     TYPE_TILE,  3.5f,       TYPE_TOOL_PICKAXE}},
};

void Tile::init() {
    if (m_inited) return;

    m_tilesDb.try_emplace(TileID::COAL_ORE, std::make_unique<CoalOre>());
    m_tilesDb.try_emplace(TileID::DIAMOND_ORE, std::make_unique<DiamondOre>());
    m_tilesDb.try_emplace(TileID::GOLD_ORE, std::make_unique<GoldOre>());
    m_tilesDb.try_emplace(TileID::GRASS, std::make_unique<Grass>());
    m_tilesDb.try_emplace(TileID::IRON_ORE, std::make_unique<IronOre>());
    m_tilesDb.try_emplace(TileID::LEAVES, std::make_unique<Leaves>());
    m_tilesDb.try_emplace(TileID::STONE, std::make_unique<Stone>());

    m_inited = true;
}

Tile* Tile::get(TileID id) {
    auto it = m_tilesDb.find(id);

    if (it != m_tilesDb.end()) {
        return it->second.get();
    }

    return nullptr;
}

float Tile::getDurability(TileID id) {
    auto it = TilesDB.find(id);
    return (it != TilesDB.end() ? it->second.durability : 0.f);
}
std::string_view Tile::getName(TileID id) {
    auto it = TilesDB.find(id);
    return (it != TilesDB.end() ? it->second.name : "Unknown");
}