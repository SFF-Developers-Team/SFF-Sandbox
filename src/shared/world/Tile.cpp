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
    float durability;
    ItemType toolType;
    bool phantom;
};

bool Tile::m_inited = false;
std::unordered_map<TileID, std::unique_ptr<Tile>> Tile::m_tilesDb;

//   id                   name            type        durability  best tool           phantom
std::unordered_map<TileID, TileProperties> TilesDB = {
    {TileID::GRASS,       {"Grass",        0.6f,       TYPE_TOOL_SHOVEL,  false}},
    {TileID::DIRT,        {"Dirt",         0.5f,       TYPE_TOOL_SHOVEL,  false}},
    {TileID::STONE,       {"Stone",        1.5f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::COBBLESTONE, {"Cobblestone",  2.0f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::PLANKS,      {"Planks",       2.0f,       TYPE_TOOL_AXE,     false}},
    {TileID::WOOL,        {"Wool",         0.8f,       TYPE_ITEM,         false}},
    {TileID::BEDROCK,     {"Bedrock",     -1.0f,       TYPE_ITEM,         false}},
    {TileID::BRICKS,      {"Bricks",       2.0f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::OAK_LOG,     {"Oak log",      2.0f,       TYPE_TOOL_AXE,     false}},
    {TileID::LEAVES,      {"Leaves",       0.2f,       TYPE_ITEM,         false}},
    {TileID::COAL_ORE,    {"Coal ore",     3.0f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::IRON_ORE,    {"Iron ore",     3.0f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::GOLD_ORE,    {"Gold ore",     3.0f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::DIAMOND_ORE, {"Diamond ore",  3.0f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::BOOKSHELF,   {"Bookshelf",    2.0f,       TYPE_TOOL_AXE,     false}},
    {TileID::FLOWER_POT,  {"Flower pot",   0.0f,       TYPE_ITEM,         true}},
    {TileID::FURNACE,     {"Furnace",      3.5f,       TYPE_TOOL_PICKAXE, false}},
    {TileID::CHEST,       {"Chest",        2.0f,       TYPE_TOOL_AXE,     true}},
    {TileID::ROSE,        {"Rose",         0.0f,       TYPE_ITEM,         true}},
    {TileID::CACTUS_HEAD, {"Cactus Head",  0.5f,       TYPE_TOOL_AXE,     true}},
    {TileID::CACTUS_BASE, {"Cactus Base",  0.5f,       TYPE_TOOL_AXE,     true}},
    {TileID::TALL_GRASS,  {"Tall Grass",   0.0f,       TYPE_ITEM,         true}},
    {TileID::MEDIUM_GRASS,{"Medium Grass", 0.0f,       TYPE_ITEM,         true}},
    {TileID::TINY_GRASS,  {"Tiny Grass",   0.0f,       TYPE_ITEM,         true}},
    {TileID::DEAD_ROSE,   {"Dead Rose",    0.0f,       TYPE_ITEM,         true}},
    {TileID::BED,         {"Bed",          2.0f,       TYPE_TOOL_AXE,     true}},
    {TileID::SIGN,        {"Sign",         1.0f,       TYPE_TOOL_AXE,     true}},
    {TileID::SNOW_CARPET, {"Snow Carpet",  0.1f,       TYPE_TOOL_SHOVEL,  true}},
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

bool Tile::isPhantom(TileID id) {
    auto it = TilesDB.find(id);
    return (it != TilesDB.end() ? it->second.phantom : false);
}

std::string_view Tile::getName(TileID id) {
    auto it = TilesDB.find(id);
    return (it != TilesDB.end() ? it->second.name : "Unknown");
}