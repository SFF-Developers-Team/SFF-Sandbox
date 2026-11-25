#include "Tile.hpp"
#include "tiles/Leaves.hpp"
#include "tiles/Grass.hpp"
#include "tiles/Cactus.hpp"
#include <optional>

struct TileProperties {
    std::string_view name;
    float durability;
    ItemType bestTool;
    bool phantom;
    std::optional<ItemID> drop;
    bool transparent = false;
    bool needsCorrectTool = false;
};

bool Tile::m_inited = false;
std::unordered_map<TileID, std::unique_ptr<Tile>> Tile::m_tileObjectDb;

// id                          name          hardness   tool           phantom  drop                trans  requires tool
const std::unordered_map<TileID, TileProperties> TilesDB = {
    {TileID::AIR,             {"Air",          0.f,  TYPE_ITEM,         true,  std::nullopt,        true}},
    {TileID::GRASS,           {"Grass",        0.6f, TYPE_TOOL_SHOVEL,  false, ItemID::DIRT,        false}},
    {TileID::DIRT,            {"Dirt",         0.5f, TYPE_TOOL_SHOVEL,  false, ItemID::DIRT,        false}},
    {TileID::STONE,           {"Stone",        1.5f, TYPE_TOOL_PICKAXE, false, ItemID::COBBLESTONE, false, true}},
    {TileID::COBBLESTONE,     {"Cobblestone",  2.0f, TYPE_TOOL_PICKAXE, false, ItemID::COBBLESTONE, false}},
    {TileID::PLANKS,          {"Planks",       2.0f, TYPE_TOOL_AXE,     false, ItemID::PLANKS,      false}},
    {TileID::WOOL,            {"Wool",         0.8f, TYPE_ITEM,         false, ItemID::WOOL,        false}},
    {TileID::BEDROCK,         {"Bedrock",     -1.0f, TYPE_ITEM,         false, std::nullopt,        false}},
    {TileID::BRICKS,          {"Bricks",       2.0f, TYPE_TOOL_PICKAXE, false, ItemID::BRICKS,      false}},
    {TileID::OAK_LOG,         {"Oak log",      2.0f, TYPE_TOOL_AXE,     false, ItemID::OAK_LOG,     false}},
    {TileID::LEAVES,          {"Leaves",       0.2f, TYPE_ITEM,         false, std::nullopt,        true}},
    {TileID::BOOKSHELF,       {"Bookshelf",    2.0f, TYPE_TOOL_AXE,     false, ItemID::BOOKSHELF,   false}},
    {TileID::FLOWER_POT,      {"Flower pot",   0.0f, TYPE_ITEM,         true,  ItemID::FLOWER_POT,  true}},
    {TileID::FURNACE,         {"Furnace",      3.5f, TYPE_TOOL_PICKAXE, false, ItemID::FURNACE,     false}},
    {TileID::ACTIVE_FURHANCE, {"Lit furnace",  3.5f, TYPE_TOOL_PICKAXE, false, ItemID::FURNACE,     false}},
    {TileID::WORKBENCH,       {"Workbench",    2.5f, TYPE_TOOL_AXE,     false, ItemID::WORKBENCH,   false}},
    {TileID::CHEST,           {"Chest",        2.0f, TYPE_TOOL_AXE,     true,  ItemID::CHEST,       true}},
    {TileID::BED,             {"Bed",          2.0f, TYPE_TOOL_AXE,     true,  ItemID::BED,         true}},
    {TileID::DEAD_ROSE,       {"Dead rose",    0.0f, TYPE_ITEM,         true,  ItemID::DEAD_ROSE,   true}},
    {TileID::ROSE,            {"Rose",         0.0f, TYPE_ITEM,         true,  ItemID::ROSE,        true}},
    {TileID::IRON_ORE,        {"Iron ore",     3.0f, TYPE_TOOL_PICKAXE, false, ItemID::IRON_ORE,    false, true}},
    {TileID::COAL_ORE,        {"Coal ore",     3.0f, TYPE_TOOL_PICKAXE, false, ItemID::COAL,        false, true}},
    {TileID::DIAMOND_ORE,     {"Diamond ore",  3.0f, TYPE_TOOL_PICKAXE, false, ItemID::DIAMOND,     false, true}},
    {TileID::GOLD_ORE,        {"Gold ore",     3.0f, TYPE_TOOL_PICKAXE, false, ItemID::GOLD_ORE,    false, true}},
    {TileID::TORCH,           {"Torch",        0.0f, TYPE_ITEM,         true,  ItemID::TORCH,       true}},
    {TileID::GLASS,           {"Glass",        0.3f, TYPE_ITEM,         false, std::nullopt,        false}},
    {TileID::SIGN,            {"Sign",         1.0f, TYPE_TOOL_AXE,     true,  ItemID::SIGN,        true}},
    {TileID::SNOW,            {"Snow",         0.5f, TYPE_TOOL_SHOVEL,  false, ItemID::SNOW,        false}},
    {TileID::SNOW_CARPET,     {"Snow carpet",  0.1f, TYPE_TOOL_SHOVEL,  true,  std::nullopt,        true}},
    {TileID::SAND,            {"Sand",         0.5f, TYPE_TOOL_SHOVEL,  false, ItemID::SAND,        false}},
    {TileID::CACTUS_HEAD,     {"Cactus head",  0.5f, TYPE_TOOL_AXE,     true,  std::nullopt,        true}},
    {TileID::CACTUS_BASE,     {"Cactus",       0.5f, TYPE_TOOL_AXE,     true,  std::nullopt,        true}},
    {TileID::TALL_GRASS,      {"Tall grass",   0.0f, TYPE_ITEM,         true,  std::nullopt,        true}},
    {TileID::MEDIUM_GRASS,    {"Medium grass", 0.0f, TYPE_ITEM,         true,  std::nullopt,        true}},
    {TileID::TINY_GRASS,      {"Tiny grass",   0.0f, TYPE_ITEM,         true,  std::nullopt,        true}},
};

const TileProperties* findProperties(TileID id) {
    const auto it = TilesDB.find(id);
    return it == TilesDB.end() ? nullptr : &it->second;
}

Tile* Tile::Get(TileID id) {
    const auto it = m_tileObjectDb.find(id);
    return it == m_tileObjectDb.end() ? nullptr : it->second.get();
}

void Tile::init() {
    if (m_inited) return;

    m_tileObjectDb.try_emplace(TileID::LEAVES, std::make_unique<Leaves>());
    m_tileObjectDb.try_emplace(TileID::GRASS, std::make_unique<Grass>());
    m_tileObjectDb.try_emplace(TileID::CACTUS_BASE, std::make_unique<Cactus>());
    m_tileObjectDb.try_emplace(TileID::CACTUS_HEAD, std::make_unique<Cactus>());

    m_inited = true;
}

float Tile::GetDurability(TileID id) {
    if (auto* tile = findProperties(id)) return tile->durability; 
    return 0.f;
}

float Tile::GetBreakingSpeed(TileID id, Item* tool) {
    auto* prop = findProperties(id);

    if (tool != nullptr && prop->bestTool == Item::GetType(tool->getID())) {
        return Item::GetToolSpeed(tool->getID());
    }

    return 1.f;
}

bool Tile::IsPhantom(TileID id) {
    if (auto* tile = findProperties(id)) return tile->phantom; 
    return false;
}

bool Tile::IsTransparent(TileID id) {
    if (auto* tile = findProperties(id)) return tile->transparent; 
    return false;
}

std::string_view Tile::GetName(TileID id) {
    if (auto* tile = findProperties(id)) return tile->name; 
    return "Unknown";
}

std::optional<ItemID> Tile::GetDrop(TileID id, Item* tool) {
    auto* p = findProperties(id);

    if (!p || !p->drop || p->durability < 0.f) return std::nullopt;
    if (p->needsCorrectTool && (!tool || Item::GetType(tool->getID()) != p->bestTool)) return std::nullopt;
    return p->drop;
}