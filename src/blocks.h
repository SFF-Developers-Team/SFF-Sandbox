#ifndef BLOCKS_H
#define BLOCKS_H

typedef enum {
    AIR,
    GRASS,
    DIRT,
    STONE,
    COBBLESTONE,
    PLANKS,
    WOOL,
    BEDROCK,
    BRICKS,
    LOG,
    LOG_FRONT,
    LEAVES,
    BOOKSHELF,
    FLOWER_POT,
    FURNACE,
    FURNACE_ACTIVE,
    WORKBENCH,
    CHEST,
    BED_1, BED_2,
    DEAD_ROSE,
    ROSE,
    IRON_ORE,
    COAL_ORE,
    DIAMOND_ORE,
    GOLD_ORE,
} Block;

typedef struct { int x; int y; } BlockPosisiton;

#define INVALID_BLOCK_POSITION ((BlockPosisiton){-1, -1})

#endif