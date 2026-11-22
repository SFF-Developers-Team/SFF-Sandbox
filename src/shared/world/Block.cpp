#include "Block.hpp"
#include "blocks/CoalOre.hpp"
#include "blocks/DiamondOre.hpp"
#include "blocks/GoldOre.hpp"
#include "blocks/Grass.hpp"
#include "blocks/IronOre.hpp"
#include "blocks/Leaves.hpp"
#include "blocks/Stone.hpp"

struct BlockInfo {
    std::string_view name;
    float durability;
    MaterialType material;
};

bool Block::m_inited = false;
std::unordered_map<BlockID, std::unique_ptr<Block>> Block::m_blocksDb;

std::unordered_map<BlockID, BlockInfo> BlocksDB = {
    {BlockID::GRASS, {
        .name = "Grass", 
       //.type = TYPE_BLOCK,
        //.spriteIndex = 0,
        .durability = 0.6f,
        //.bestTool = TYPE_TOOL_SHOVEL
    }},
    {BlockID::DIRT, {
        .name = "Dirt", 
       //.type = TYPE_BLOCK, 
        //.spriteIndex = 1,
        .durability = 0.5f,
        //.bestTool = TYPE_TOOL_SHOVEL
    }},
    {BlockID::STONE, {
        .name = "Stone", 
       //.type = TYPE_BLOCK, 
        //.spriteIndex = 2,
        .durability = 1.5f,
        .material = MATERIAL_STONE, 
        //.bestTool = TYPE_TOOL_PICKAXE
    }},
    {BlockID::COBBLESTONE, {
        .name = "Cobblestone", 
       //.type = TYPE_BLOCK, 
        //.spriteIndex = 3,
        .durability = 2.f,
        .material = MATERIAL_STONE, 
        //.bestTool = TYPE_TOOL_PICKAXE
    }},
    {BlockID::PLANKS, {
        .name = "Planks", 
       //.type = TYPE_BLOCK, 
        //.spriteIndex = 4,
        .durability = 2.f,
        .material = MATERIAL_WOOD, 
        //.bestTool = TYPE_TOOL_AXE
    }},
    {BlockID::WOOL, {
        .name = "Wool", 
       //.type = TYPE_BLOCK, 
        //.spriteIndex = 5,
        .durability = 0.8f,
    }},
    {BlockID::BEDROCK, {
        .name = "Bedrock", 
       //.type = TYPE_BLOCK, 
        //.spriteIndex = 6,
        .durability = -1.f,
        .material = MATERIAL_NONE, 
    }},
    {BlockID::BRICKS, {
        .name = "Bricks", 
       //.type = TYPE_BLOCK, 
        //.spriteIndex = 7,
        .durability = 2.f,
        .material = MATERIAL_STONE, 
        //.bestTool = TYPE_TOOL_PICKAXE
    }},
    {BlockID::OAK_LOG, {
        .name = "Oak log",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 8,
        .durability = 2.f,
        .material = MATERIAL_WOOD,
        //.bestTool = TYPE_TOOL_AXE
    }},
    {BlockID::LEAVES, {
        .name = "Leaves",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 10,
        .durability = 0.2f,
        .material = MATERIAL_NONE,
    }},
    {BlockID::COAL_ORE, {
        .name = "Coal ore",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 11,
        .durability = 3.f,
        .material = MATERIAL_STONE,
        //.bestTool = TYPE_TOOL_PICKAXE
    }},
    {BlockID::IRON_ORE, {
        .name = "Iron ore",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 12,
        .durability = 3.f,
        .material = MATERIAL_STONE,
        //.bestTool = TYPE_TOOL_PICKAXE,
        //.bestToolMaterial = MATERIAL_STONE
    }},
    {BlockID::GOLD_ORE, {
        .name = "Gold ore",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 13,
        .durability = 3.f,
        .material = MATERIAL_STONE,
        //.bestTool = TYPE_TOOL_PICKAXE,
        //.bestToolMaterial = MATERIAL_IRON
    }},
    {BlockID::DIAMOND_ORE, {
        .name = "Diamond ore",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 14,
        .durability = 3.f,
        .material = MATERIAL_STONE,
        //.bestTool = TYPE_TOOL_PICKAXE,
        //.bestToolMaterial = MATERIAL_IRON
    }},
    {BlockID::LAPIZ_ORE, {
        .name = "Lapiz ore",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 15,
        .durability = 3.f,
        .material = MATERIAL_STONE,
        //.bestTool = TYPE_TOOL_PICKAXE,
        //.bestToolMaterial = MATERIAL_STONE
    }},
    {BlockID::BOOKSHELF, {
        .name = "Bookshelf",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 16,
        .durability = 2.f,
        .material = MATERIAL_WOOD,
        //.bestTool = TYPE_TOOL_AXE
    }},
    {BlockID::FLOWER_POT, {
        .name = "Flower pot",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 17,
        .durability = 0.f,
    }},
    {BlockID::FURNACE, {
        .name = "Furnace",
       //.type = TYPE_BLOCK,
        //.spriteIndex = 18,
        .durability = 3.5f,
        .material = MATERIAL_STONE,
        //.bestTool = TYPE_TOOL_PICKAXE,
    }},
};

void Block::init() {
    if (m_inited) return;

    m_blocksDb.try_emplace(BlockID::COAL_ORE, std::make_unique<CoalOre>());
    m_blocksDb.try_emplace(BlockID::DIAMOND_ORE, std::make_unique<DiamondOre>());
    m_blocksDb.try_emplace(BlockID::GOLD_ORE, std::make_unique<GoldOre>());
    m_blocksDb.try_emplace(BlockID::GRASS, std::make_unique<Grass>());
    m_blocksDb.try_emplace(BlockID::IRON_ORE, std::make_unique<IronOre>());
    m_blocksDb.try_emplace(BlockID::LEAVES, std::make_unique<Leaves>());
    m_blocksDb.try_emplace(BlockID::STONE, std::make_unique<Stone>());

    m_inited = true;
}

Block* Block::get(BlockID id) {
    auto it = m_blocksDb.find(id);

    if (it != m_blocksDb.end()) {
        return it->second.get();
    }

    return nullptr;
}

float Block::getDurability(BlockID id) {
    auto it = BlocksDB.find(id);
    return (it != BlocksDB.end() ? it->second.durability : 0.f);
}