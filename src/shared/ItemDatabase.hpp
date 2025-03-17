#pragma once
#include <string>
#include <Types.hpp>
#include <unordered_map>

struct ItemInfo {
    std::string name;
    ItemType type = TYPE_ITEM;
    MaterialType material = MATERIAL_NONE;
    uint8_t spriteIndex = 0;
    float durability = 0.f;
    ItemTags defaultTags;
    ItemType bestTool = (ItemType)0;
    MaterialType bestToolMaterial = MATERIAL_NONE;
};

inline std::unordered_map<ItemID, ItemInfo> gItemDatabase = {
    {GRASS, {
        .name = "Grass", 
        .type = TYPE_BLOCK,
        .spriteIndex = 0,
        .durability = 0.6f,
        .bestTool = TYPE_TOOL_SHOVEL
    }},
    {DIRT, {
        .name = "Dirt", 
        .type = TYPE_BLOCK, 
        .spriteIndex = 1,
        .durability = 0.5f,
        .bestTool = TYPE_TOOL_SHOVEL
    }},
    {STONE, {
        .name = "Stone", 
        .type = TYPE_BLOCK, 
        .material = MATERIAL_STONE, 
        .spriteIndex = 2,
        .durability = 1.5f,
        .bestTool = TYPE_TOOL_PICKAXE
    }},
    {COBBLESTONE, {
        .name = "Cobblestone", 
        .type = TYPE_BLOCK, 
        .material = MATERIAL_STONE, 
        .spriteIndex = 3,
        .durability = 2.f,
        .bestTool = TYPE_TOOL_PICKAXE
    }},
    {PLANKS, {
        .name = "Planks", 
        .type = TYPE_BLOCK, 
        .material = MATERIAL_WOOD, 
        .spriteIndex = 4,
        .durability = 2.f,
        .bestTool = TYPE_TOOL_AXE
    }},
    {WOOL, {
        .name = "Wool", 
        .type = TYPE_BLOCK, 
        .spriteIndex = 5,
        .durability = 0.8f,
    }},
    {BEDROCK, {
        .name = "Bedrock", 
        .type = TYPE_BLOCK, 
        .material = MATERIAL_NONE, 
        .spriteIndex = 6,
        .durability = -1.f
    }},
    {BRICKS, {
        .name = "Bricks", 
        .type = TYPE_BLOCK, 
        .material = MATERIAL_STONE, 
        .spriteIndex = 7,
        .durability = 2.f,
        .bestTool = TYPE_TOOL_PICKAXE
    }},
    {OAK_LOG, {
        .name = "Oak log",
        .type = TYPE_BLOCK,
        .material = MATERIAL_WOOD,
        .spriteIndex = 8,
        .durability = 2.f,
        .bestTool = TYPE_TOOL_AXE
    }},
    {LEAVES, {
        .name = "Leaves",
        .type = TYPE_BLOCK,
        .material = MATERIAL_NONE,
        .spriteIndex = 10,
        .durability = 0.2f
    }},
    {COAL_ORE, {
        .name = "Coal ore",
        .type = TYPE_BLOCK,
        .material = MATERIAL_STONE,
        .spriteIndex = 11,
        .durability = 3.f,
        .bestTool = TYPE_TOOL_PICKAXE
    }},
    {IRON_ORE_BLOCK, {
        .name = "Iron ore",
        .type = TYPE_BLOCK,
        .material = MATERIAL_STONE,
        .spriteIndex = 12,
        .durability = 3.f,
        .bestTool = TYPE_TOOL_PICKAXE,
        .bestToolMaterial = MATERIAL_STONE
    }},
    {GOLD_ORE_BLOCK, {
        .name = "Gold ore",
        .type = TYPE_BLOCK,
        .material = MATERIAL_STONE,
        .spriteIndex = 13,
        .durability = 3.f,
        .bestTool = TYPE_TOOL_PICKAXE,
        .bestToolMaterial = MATERIAL_IRON
    }},
    {DIAMOND_ORE, {
        .name = "Diamond ore",
        .type = TYPE_BLOCK,
        .material = MATERIAL_STONE,
        .spriteIndex = 14,
        .durability = 3.f,
        .bestTool = TYPE_TOOL_PICKAXE,
        .bestToolMaterial = MATERIAL_IRON
    }},
    {LAPIZ_ORE, {
        .name = "Lapiz ore",
        .type = TYPE_BLOCK,
        .material = MATERIAL_STONE,
        .spriteIndex = 15,
        .durability = 3.f,
        .bestTool = TYPE_TOOL_PICKAXE,
        .bestToolMaterial = MATERIAL_STONE
    }},
    {BOOKSHELF, {
        .name = "Bookshelf",
        .type = TYPE_BLOCK,
        .material = MATERIAL_WOOD,
        .spriteIndex = 16,
        .durability = 2.f,
        .bestTool = TYPE_TOOL_AXE
    }},
    {FLOWER_POT, {
        .name = "Flower pot",
        .type = TYPE_BLOCK,
        .spriteIndex = 17,
        .durability = 0.f,
    }},
    {FURNACE, {
        .name = "Furnace",
        .type = TYPE_BLOCK,
        .material = MATERIAL_STONE,
        .spriteIndex = 18,
        .durability = 3.5f,
        .bestTool = TYPE_TOOL_PICKAXE,
    }},
    {STICK, {
        .name = "Stick",
        .type = TYPE_ITEM,
        .material = MATERIAL_NONE,
        .spriteIndex = 0
    }},
    {WOODEN_PICKAXE, {
        .name = "Wooden pickaxe",
        .type = TYPE_TOOL_PICKAXE,
        .material = MATERIAL_NONE,
        .spriteIndex = 1,
        .durability = 59.f,
        .defaultTags = {{TAG_EFFICIENCY, (uint8_t)2}}
    }},
    {STONE_PICKAXE, {
        .name = "Stone pickaxe",
        .type = TYPE_TOOL_PICKAXE,
        .material = MATERIAL_NONE,
        .spriteIndex = 3,
        .durability = 131.f,
        .defaultTags = {{TAG_EFFICIENCY, (uint8_t)5}}
    }},
    {IRON_PICKAXE, {
        .name = "Iron pickaxe",
        .type = TYPE_TOOL_PICKAXE,
        .material = MATERIAL_NONE,
        .spriteIndex = 5,
        .durability = 250.f,
        .defaultTags = {{TAG_EFFICIENCY, (uint8_t)10}}
    }},
    {GOLDEN_PICKAXE, {
        .name = "Golden pickaxe",
        .type = TYPE_TOOL_PICKAXE,
        .spriteIndex = 7,
        .durability = 33.f,
        .defaultTags = {{TAG_EFFICIENCY, (uint8_t)20}}
    }},
    {DIAMOND_PICKAXE, {
        .name = "Diamond pickaxe",
        .type = TYPE_TOOL_PICKAXE,
        .material = MATERIAL_NONE,
        .spriteIndex = 9,
        .durability = 1516.f,
        .defaultTags = {{TAG_EFFICIENCY, (uint8_t)15}}
    }},
    {WOODEN_SWORD, {
        .name = "Wooden sword",
        .type = TYPE_WEAPON,
        .material = MATERIAL_NONE,
        .spriteIndex = 2,
        .durability = 59.f
    }},
    {STONE_SWORD, {
        .name = "Stone sword",
        .type = TYPE_WEAPON,
        .material = MATERIAL_NONE,
        .spriteIndex = 4,
        .durability = 131.f
    }},
    {IRON_SWORD, {
        .name = "Iron sword",
        .type = TYPE_WEAPON,
        .material = MATERIAL_NONE,
        .spriteIndex = 6,
        .durability = 250.f
    }},
    {GOLDEN_SWORD, {
        .name = "Golden sword",
        .type = TYPE_WEAPON,
        .material = MATERIAL_NONE,
        .spriteIndex = 8,
        .durability = 32.f
    }},
    {DIAMOND_SWORD, {
        .name = "Diamond sword",
        .type = TYPE_WEAPON,
        .material = MATERIAL_NONE,
        .spriteIndex = 10,
        .durability = 1561.f
    }},
    {COAL, {
        .name = "Coal",
        .type = TYPE_ITEM,
        .material = MATERIAL_NONE,
        .spriteIndex = 14
    }},
    {IRON_INGOT, {
        .name = "Iron",
        .type = TYPE_ITEM,
        .material = MATERIAL_NONE,
        .spriteIndex = 15
    }},
    {GOLD_INGOT, {
        .name = "Gold",
        .type = TYPE_ITEM,
        .material = MATERIAL_NONE,
        .spriteIndex = 16
    }},
    {DIAMOND, {
        .name = "Diamond",
        .type = TYPE_ITEM,
        .material = MATERIAL_NONE,
        .spriteIndex = 17
    }}  
};