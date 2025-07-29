#include "Default.hpp"
#include "Blocks.hpp"
#include "Chunk.hpp"
#include "Types.hpp"

void Default::GenerateChunk(Chunk& chunk, Vector2i pos) {
    // Don't generate sky chunks
    if (pos.y < 0) {
        return;
    }

    // Generate terrain
    for (int z = 0; z < CHUNK_DEPTH; z++) {
        int worldX = CHUNK_WIDTH * pos.x;
        for (int x = 0; x < CHUNK_WIDTH; ++x, ++worldX) {
            int surface = 150 + 20.f * m_perlin.octave2D_01(worldX * 0.01, z * 0.1f, 4);
            int ground = m_perlin.octave2D_01(worldX * 0.1, z * 0.1f, 2) * 8;
            float roseNoise = m_perlin.octave2D_01(x * 0.3, 0, 1);

            int const stoneLayer = surface + 10 + ground;

            int worldY = CHUNK_HEIGHT * pos.y;
            for (int y = 0; y < CHUNK_HEIGHT; ++y, ++worldY) {
                if (worldY > surface) {
                    chunk.SetBlock(x, y, z, (worldY < stoneLayer) ? BLOCK_ID_DIRT : BLOCK_ID_STONE);
                } else if (worldY == surface) {
                    chunk.SetBlock(x, y, z, BLOCK_ID_GRASS);

                    if (roseNoise > 0.7f && y > 0 && z == CHUNK_FOREGROUND_LAYER) {
                        chunk.SetBlock(x, y - 1, z, BLOCK_ID_ROSE);
                    }
                }
            }
        }
    }

    // Generate caves
    for (int x = 0; x < CHUNK_WIDTH; x++) { 
        int worldX = x + CHUNK_WIDTH * pos.x;

        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            int worldY = y + CHUNK_HEIGHT * pos.y;

            float cave = m_perlin.octave2D_01(worldX * 0.05, worldY * 0.05, 3);

            if (cave < worldY / 1000.f) {
                chunk.SetBlock(x, y, 1, BLOCK_ID_AIR);
            }
        }
    }

    // Generate lakes
    // Coming soon...

    // Generate hell
    // Coming soon...
}