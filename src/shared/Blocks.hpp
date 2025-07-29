#pragma once

#include <cstdint>
#include <unordered_set>
#include <array>
#include <Types.hpp>

typedef enum : uint8_t {
    BLOCK_ID_AIR,
    BLOCK_ID_GRASS,
    BLOCK_ID_DIRT,
    BLOCK_ID_STONE,
    BLOCK_ID_COBBLESTONE,
    BLOCK_ID_PLANKS,
    BLOCK_ID_WOOL,
    BLOCK_ID_ADMINIUM, // :D
    BLOCK_ID_BRICKS,
    BLOCK_ID_WOOD_1,
    BLOCK_ID_WOOD_2,
    BLOCK_ID_LEAVES,
    BLOCK_ID_BOOKSHELF,
    BLOCK_ID_FLOWER_POT,
    BLOCK_ID_FURNACE_1,
    BLOCK_ID_FURNACE_2,
    BLOCK_ID_WORKBENCH,
    BLOCK_ID_CHEST,
    BLOCK_ID_BED_1,
    BLOCK_ID_BED_2,
    BLOCK_ID_DEAD_ROSE,
    BLOCK_ID_ROSE,
    BLOCK_ID_IRON_ORE,
    BLOCK_ID_COAL_ORE,
    BLOCK_ID_DIAMOND_ORE,
    BLOCK_ID_GOLD_ORE,
    BLOCK_ID_TORCH_1,
    BLOCK_ID_TORCH_2,
    BLOCK_ID_TORCH_3,
    BLOCK_ID_TORCH_4,
    BLOCK_ID_TORCH_5,
    BLOCK_ID_TORCH_6,
    BLOCK_ID_TORCH_7,
    BLOCK_ID_TORCH_8,
    BLOCK_ID_GLASS,
    BLOCK_ID_SIGN
} BlockID;

std::unordered_set<BlockID> inline const gTransperentBlocks = {
    BLOCK_ID_LEAVES, BLOCK_ID_FURNACE_1, BLOCK_ID_FURNACE_2, BLOCK_ID_WORKBENCH, BLOCK_ID_CHEST, BLOCK_ID_FLOWER_POT,
    BLOCK_ID_BED_1, BLOCK_ID_BED_2, BLOCK_ID_DEAD_ROSE, BLOCK_ID_ROSE, BLOCK_ID_TORCH_1, BLOCK_ID_TORCH_2, BLOCK_ID_TORCH_3,
    BLOCK_ID_TORCH_4, BLOCK_ID_TORCH_5, BLOCK_ID_TORCH_6, BLOCK_ID_TORCH_7, BLOCK_ID_TORCH_8
};

std::unordered_set<BlockID> inline const gGhostBlocks = {
    BLOCK_ID_AIR, BLOCK_ID_FLOWER_POT, BLOCK_ID_BED_1, BLOCK_ID_BED_2, BLOCK_ID_DEAD_ROSE, BLOCK_ID_ROSE, BLOCK_ID_TORCH_1, BLOCK_ID_TORCH_2,
    BLOCK_ID_TORCH_3, BLOCK_ID_TORCH_4, BLOCK_ID_TORCH_5, BLOCK_ID_TORCH_6, BLOCK_ID_TORCH_7, BLOCK_ID_TORCH_8
};


constexpr auto blocksDatabase = std::to_array<BlockInfo>({
   //BLOCK_AIR
   {
      .hardness = 0.f,
      .toolType = TYPE_ALL
   },
   //BLOCK_GRASS
   {
      .hardness = 5.f,
      .toolType = TYPE_SHOVEL,
      .fabricID = ID_BLOCKITEM_GRASS
   },
   //BLOCK_DIRT
   {
      .hardness = 5.f,
      .toolType = TYPE_SHOVEL,
      .fabricID = ID_BLOCKITEM_DIRT

   },
   //BLOCK_STONE
   {
      .hardness = 20.f,
      .toolType = TYPE_PICKAXE
   },
   //BLOCK_COBBLESTONE
   {
      .hardness = 20.f,
      .toolType = TYPE_PICKAXE
   },
   //BLOCK_ID_PLANKS
   {
      .hardness = 13.f,
      .toolType = TYPE_AXE
   },
   //BLOCK_ID_WOOL
   {
      .hardness = 10.f,
      .toolType = TYPE_SCISSORS
   },
   //BLOCK_ID_ADMINIUM
   {
      .hardness = 50.f,
      .toolType = TYPE_PICKAXE
   },
   //BLOCK_ID_BRICKS,
   {
      .hardness = 20.f,
      .toolType = TYPE_PICKAXE
   },
   //  BLOCK_ID_WOOD_1
   {
      .hardness = 20.f,
      .toolType = TYPE_AXE
   },
   //  BLOCK_ID_WOOD_2
   {
      .hardness = 20.f,
      .toolType = TYPE_AXE
   },
   //  BLOCK_ID_LEAVES
   {
      .hardness = 5.f,
      .toolType = TYPE_SCISSORS
   },
   //  BLOCK_ID_BOOKSHELF
   {
      .hardness = 15.f,
      .toolType = TYPE_AXE
   },
   //  BLOCK_ID_FLOWER_POT
   {
      .hardness = 5.f,
      .toolType = TYPE_ALL
   },
   //  BLOCK_ID_FURNACE_1
   {
      .hardness = 20.f,
      .toolType = TYPE_PICKAXE
   },
   //  BLOCK_ID_FURNACE_2
   {
      .hardness = 20.f,
      .toolType = TYPE_PICKAXE
   },
   //  BLOCK_ID_WORKBENCH
   {
      .hardness = 20.f,
      .toolType = TYPE_AXE
   },
   //  BLOCK_ID_CHEST
   {
      .hardness = 20.f,
      .toolType = TYPE_AXE
   },
   //  BLOCK_ID_BED_1
   {
      .hardness = 15.f,
      .toolType = TYPE_AXE
   },
   //  BLOCK_ID_BED_2
   {
      .hardness = 15.f,
      .toolType = TYPE_AXE
   },
   //  BLOCK_ID_DEAD_ROSE
   {
      .hardness = 5.f,
      .toolType = TYPE_ALL
   },
   //  BLOCK_ID_ROSE
   {
      .hardness = 5.f,
      .toolType = TYPE_ALL
   },
   //  BLOCK_ID_IRON_ORE
   {
      .hardness = 35.f,
      .toolType = TYPE_PICKAXE
   },
   //  BLOCK_ID_COAL_ORE
   {
      .hardness = 20.f,
      .toolType = TYPE_PICKAXE
   },
   //  BLOCK_ID_DIAMOND_ORE
   {
      .hardness = 50.f,
      .toolType = TYPE_PICKAXE
   },
   //  BLOCK_ID_GOLD_ORE
   {
      .hardness = 40.f,
      .toolType = TYPE_PICKAXE
   },
   //  BLOCK_ID_TORCH_1
   {
      .hardness = 5.f,
      .toolType = TYPE_ALL
   },
});
namespace BlockData {
   inline float GetHardness(BlockID id) { return blocksDatabase.at(id).hardness; }
   inline float GetToolType(BlockID id) { return blocksDatabase.at(id).toolType; }
};