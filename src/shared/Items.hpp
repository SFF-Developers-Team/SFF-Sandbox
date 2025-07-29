#pragma once

#include <cstdint>
#include <unordered_set>
#include <array>
#include <Types.hpp>
#include <Blocks.hpp>

// enum Items {
//     EMPTY = 0,
//     WOODEN_PICKAXE,
//     STONE_PICKAXE,
//     IRON_PICKAXE,
//     GOLD_PICKAXE,
//     DIAMOND_PICKAXE,
//     WOODEN_SHOVEL,
//     STONE_SHOVEL,
//     IRON_SHOVEL,
//     GOLD_SHOVEL,
//     DIAMOND_SHOVEL,
//     WOODEN_AXE,
//     STONE_AXE,
//     IRON_AXE,
//     GOLD_AXE,
//     DIAMOND_AXE,
// };

// inline float CalculateItemSpeed(int8_t itemID, BlockID block) {
//     ItemInfo item = itemsDatabase.at(itemID);
//     auto blocks = blocksDatabase.at(block);

//     if (blocks.toolType == TYPE_ALL) {
//         return 0.07f;
//     }
//     if (blocks.toolType != item.type) {
//         return 0.01f;
//     }
    
//     return item.speed / blocks.hardness;
// }

