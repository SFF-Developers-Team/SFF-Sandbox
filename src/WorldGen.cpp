#include <WorldGen.hpp>
#include <world.hpp>
#include <stdio.h>

WorldGen::WorldGen(World *world) {
    m_world = world;
} 

void WorldGen::generateWorld() {
    for (int x = 0; x < m_world->m_width; x++) {
        for (int y = 0; y < m_world->m_height; y++) {
            int index = x * m_world->m_height + y;

            auto block = generateBlock(x, y);

            // printf("x=%d; y=%d; ptr=%016X\n", x, y, block);

            m_world->m_blocks[index] = block;
        }
    }
}