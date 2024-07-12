#include <WorldGen.hpp>
#include <World.hpp>
#include <stdio.h>

WorldGen::WorldGen(World *world) {
    m_world = world;
} 

void WorldGen::generateWorld() {
    for (int x = 0; x < m_world->getWidth(); x++) {
        for (int y = 0; y < m_world->getHeight(); y++) {
            auto block = generateBlock(x, y);

            // printf("x=%d; y=%d; ptr=%016X\n", x, y, block);

            m_world->setBlock(x, y, block);
        }
    }
}