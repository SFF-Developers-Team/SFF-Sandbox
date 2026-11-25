#include "Types.hpp"
#include <Item.hpp>
#include <world/Tile.hpp>
#include <unordered_map>

struct ItemProperties {
    std::string_view name;
    ItemType type;
    ItemMaterial material;
    uint16_t spriteIndex;
    float durability;
    TileID placedTile = TileID::AIR;
};

const std::unordered_map<ItemID, ItemProperties> ItemsDB = {
    // ID                      Name              Type               Material            Sprite  Durability  Placed tile
    {ItemID::GRASS,           {"Grass",          TYPE_TILE,         ItemMaterial::NONE,        0,      0.f, TileID::GRASS}},
    {ItemID::DIRT,            {"Dirt",           TYPE_TILE,         ItemMaterial::NONE,        1,      0.f, TileID::DIRT}},
    {ItemID::STONE,           {"Stone",          TYPE_TILE,         ItemMaterial::STONE,       2,      0.f, TileID::STONE}},
    {ItemID::COBBLESTONE,     {"Cobblestone",    TYPE_TILE,         ItemMaterial::STONE,       3,      0.f, TileID::COBBLESTONE}},
    {ItemID::PLANKS,          {"Planks",         TYPE_TILE,         ItemMaterial::WOOD,        4,      0.f, TileID::PLANKS}},
    {ItemID::WOOL,            {"Wool",           TYPE_TILE,         ItemMaterial::NONE,        5,      0.f, TileID::WOOL}},
    {ItemID::BEDROCK,         {"Bedrock",        TYPE_TILE,         ItemMaterial::STONE,       6,      0.f, TileID::BEDROCK}},
    {ItemID::BRICKS,          {"Bricks",         TYPE_TILE,         ItemMaterial::STONE,       7,      0.f, TileID::BRICKS}},
    {ItemID::OAK_LOG,         {"Oak log",        TYPE_TILE,         ItemMaterial::WOOD,        8,      0.f, TileID::OAK_LOG}},
    {ItemID::LEAVES,          {"Leaves",         TYPE_TILE,         ItemMaterial::NONE,       10,      0.f, TileID::LEAVES}},
    {ItemID::BOOKSHELF,       {"Bookshelf",      TYPE_TILE,         ItemMaterial::WOOD,       11,      0.f, TileID::BOOKSHELF}},
    {ItemID::FLOWER_POT,      {"Flower pot",     TYPE_TILE,         ItemMaterial::NONE,       12,      0.f, TileID::FLOWER_POT}},
    {ItemID::FURNACE,         {"Furnace",        TYPE_TILE,         ItemMaterial::STONE,      13,      0.f, TileID::FURNACE}},
    {ItemID::WORKBENCH,       {"Workbench",      TYPE_TILE,         ItemMaterial::WOOD,       15,      0.f, TileID::WORKBENCH}},
    {ItemID::CHEST,           {"Chest",          TYPE_TILE,         ItemMaterial::WOOD,       16,      0.f, TileID::CHEST}},
    {ItemID::BED,             {"Bed",            TYPE_TILE,         ItemMaterial::WOOD,       17,      0.f, TileID::BED}},
    {ItemID::DEAD_ROSE,       {"Dead rose",      TYPE_TILE,         ItemMaterial::NONE,       19,      0.f, TileID::DEAD_ROSE}},
    {ItemID::ROSE,            {"Rose",           TYPE_TILE,         ItemMaterial::NONE,       20,      0.f, TileID::ROSE}},
    {ItemID::TORCH,           {"Torch",          TYPE_TILE,         ItemMaterial::NONE,       25,      0.f, TileID::TORCH}},
    {ItemID::GLASS,           {"Glass",          TYPE_TILE,         ItemMaterial::NONE,       33,      0.f, TileID::GLASS}},
    {ItemID::SIGN,            {"Sign",           TYPE_TILE,         ItemMaterial::WOOD,       34,      0.f, TileID::SIGN}},
    {ItemID::SNOW,            {"Snow",           TYPE_TILE,         ItemMaterial::NONE,       35,      0.f, TileID::SNOW}},
    {ItemID::SAND,            {"Sand",           TYPE_TILE,         ItemMaterial::NONE,       37,      0.f, TileID::SAND}},

    // Regular items
    {ItemID::STICK,           {"Stick",          TYPE_ITEM,         ItemMaterial::NONE,        0,      0.f,      TileID::AIR}},
    {ItemID::WOODEN_PICKAXE,  {"Wooden pickaxe", TYPE_TOOL_PICKAXE, ItemMaterial::WOOD,        1,     59.f,      TileID::AIR}},
    {ItemID::WOODEN_SWORD,    {"Wooden sword",   TYPE_WEAPON,       ItemMaterial::WOOD,        2,     59.f,      TileID::AIR}},
    {ItemID::STONE_PICKAXE,   {"Stone pickaxe",  TYPE_TOOL_PICKAXE, ItemMaterial::STONE,       3,    131.f,      TileID::AIR}},
    {ItemID::STONE_SWORD,     {"Stone sword",    TYPE_WEAPON,       ItemMaterial::STONE,       4,    131.f,      TileID::AIR}},
    {ItemID::IRON_PICKAXE,    {"Iron pickaxe",   TYPE_TOOL_PICKAXE, ItemMaterial::IRON,        5,    250.f,      TileID::AIR}},
    {ItemID::IRON_SWORD,      {"Iron sword",     TYPE_WEAPON,       ItemMaterial::IRON,        6,    250.f,      TileID::AIR}},
    {ItemID::GOLDEN_PICKAXE,  {"Golden pickaxe", TYPE_TOOL_PICKAXE, ItemMaterial::GOLD,        7,     33.f,      TileID::AIR}},
    {ItemID::GOLDEN_SWORD,    {"Golden sword",   TYPE_WEAPON,       ItemMaterial::GOLD,        8,     32.f,      TileID::AIR}},
    {ItemID::DIAMOND_PICKAXE, {"Diamond pickaxe",TYPE_TOOL_PICKAXE, ItemMaterial::DIAMOND,     9,   1516.f,      TileID::AIR}},
    {ItemID::DIAMOND_SWORD,   {"Diamond sword",  TYPE_WEAPON,       ItemMaterial::DIAMOND,    10,   1561.f,      TileID::AIR}},
    {ItemID::IRON_ORE,        {"Raw iron",       TYPE_ITEM,         ItemMaterial::NONE,       11,      0.f,      TileID::AIR}},
    {ItemID::GOLD_ORE,        {"Raw gold",       TYPE_ITEM,         ItemMaterial::NONE,       12,      0.f,      TileID::AIR}},
    {ItemID::COAL,            {"Coal",           TYPE_ITEM,         ItemMaterial::NONE,       14,      0.f,      TileID::AIR}},
    {ItemID::IRON_INGOT,      {"Iron ingot",     TYPE_ITEM,         ItemMaterial::NONE,       15,      0.f,      TileID::AIR}},
    {ItemID::GOLD_INGOT,      {"Gold ingot",     TYPE_ITEM,         ItemMaterial::NONE,       16,      0.f,      TileID::AIR}},
    {ItemID::DIAMOND,         {"Diamond",        TYPE_ITEM,         ItemMaterial::NONE,       17,      0.f,      TileID::AIR}}
};

const ItemProperties* findProperties(ItemID id) {
    auto it = ItemsDB.find(id);
    return it == ItemsDB.end() ? nullptr : &it->second;
}

std::string_view Item::GetName(ItemID id) {
    if (auto item = findProperties(id)) return item->name;
    return "Unknown";
}

ItemType Item::GetType(ItemID id) {
    if (auto item = findProperties(id)) return item->type;
    return ItemType::TYPE_ITEM;
}

ItemMaterial Item::GetMaterial(ItemID id) {
    if (auto item = findProperties(id)) return item->material;
    return ItemMaterial::NONE;
}

TileID Item::GetTile(ItemID id) {
    if (auto* item = findProperties(id)) return item->placedTile;
    return TileID::AIR;
}

float Item::GetToolSpeed(ItemID id) {
    auto type = GetType(id);

    if (type > TYPE_ITEM) {
        if (type == TYPE_WEAPON) {
            return 1.5f;
        }

        switch (GetMaterial(id)) {
            case ItemMaterial::WOOD: return 2.f;
            case ItemMaterial::STONE: return 4.f;
            case ItemMaterial::IRON: return 6.f;
            case ItemMaterial::DIAMOND: return 8.f;
            case ItemMaterial::GOLD: return 12.f;
            default: return 1.f;
        }
    }

    return 1.f;
}

int Item::GetSpriteIndex(ItemID id) {
    if (auto item = findProperties(id)) return item->spriteIndex;
    return 255;
}

DataStream Item::serialize() {
    auto ret = Serializable::serialize();

    ret.add(m_id);
    ret.add(m_aux);

    return ret;
}

bool Item::deserialize(DataStream& stream) {
    if(!Serializable::deserialize(stream)) return false;
    
    m_id = stream.get<ItemID>();
    m_aux = stream.get<int>();

    return true;
}