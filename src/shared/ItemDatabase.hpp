#pragma once
#include <string>
#include <Types.hpp>
#include <unordered_map>

struct BlockInfo {
    std::string name;
    ItemType type;
    MaterialType material;
    uint8_t spriteIndex;
};

inline std::unordered_map<ItemID, BlockInfo> gBlocksDatabase = {
    {GRASS, {"Grass", TYPE_BLOCK, MATERIAL_DIRT, 0}},
    {DIRT, {"Dirt", TYPE_BLOCK, MATERIAL_DIRT, 1}},
    {STONE, {"Stone", TYPE_BLOCK, MATERIAL_STONE, 2}},
    {COBBLESTONE, {"Cobblestone", TYPE_BLOCK, MATERIAL_STONE, 3}},
    {PLANKS, {"Planks", TYPE_BLOCK, MATERIAL_WOOD, 4}},
    {WOOL, {"Wool", TYPE_BLOCK, MATERIAL_WOOL, 5}},
    {BEDROCK, {"Bedrock", TYPE_BLOCK, MATERIAL_NONE, 6}},
    {BRICKS, {"Bricks", TYPE_BLOCK, MATERIAL_STONE, 7}},
    {OAK_LOG, {"Oak log", TYPE_BLOCK, MATERIAL_WOOD, 8}},
    {ANOTHER_OAK_LOG, {"Oak log", TYPE_BLOCK, MATERIAL_WOOD, 9}},
    {LEAVES, {"Leaves", TYPE_BLOCK, MATERIAL_NONE, 10}},
    {COAL_ORE, {"Coal ore", TYPE_BLOCK, MATERIAL_STONE, 11}},
    {IRON_ORE, {"Iron ore", TYPE_BLOCK, MATERIAL_STONE, 12}},
    {GOLD_ORE, {"Gold ore", TYPE_BLOCK, MATERIAL_STONE, 13}},
    {DIAMOND_ORE, {"Diamond ore", TYPE_BLOCK, MATERIAL_STONE, 14}},
    {LAPIZ_ORE, {"Lapiz ore", TYPE_BLOCK, MATERIAL_STONE, 15}},
    {BOOKSHELF, {"Bookshelf", TYPE_BLOCK, MATERIAL_WOOD, 16}},
    {FLOWER_POT, {"Flower pot", TYPE_BLOCK, MATERIAL_DIRT, 17}},
    {FURHANCE, {"Furhance", TYPE_BLOCK, MATERIAL_STONE, 18}},
    {STICK, {"Stick", TYPE_ITEM, MATERIAL_NONE, 0}},
    {WOODEN_PICKAXE, {"Wooden pickaxe", TYPE_ITEM, MATERIAL_NONE, 1}},
    {STONE_PICKAXE, {"Stone pickaxe", TYPE_ITEM, MATERIAL_NONE, 3}},
    {IRON_PICKAXE, {"Iron pickaxe", TYPE_ITEM, MATERIAL_NONE, 5}},
    {GOLDEN_PICKAXE, {"Golden pickaxe", TYPE_ITEM, MATERIAL_NONE, 7}},
    {DIAMOND_PICKAXE, {"Diamond pickaxe", TYPE_ITEM, MATERIAL_NONE, 9}},
    {WOODEN_SWORD, {"Wooden sword", TYPE_ITEM, MATERIAL_NONE, 2}},
    {STONE_SWORD, {"Stone sword", TYPE_ITEM, MATERIAL_NONE, 4}},
    {IRON_SWORD, {"Iron sword", TYPE_ITEM, MATERIAL_NONE, 6}},
    {GOLDEN_SWORD, {"Golden sword", TYPE_ITEM, MATERIAL_NONE, 8}},
    {DIAMOND_SWORD, {"Diamond sword", TYPE_ITEM, MATERIAL_NONE, 10}}
};