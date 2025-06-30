#include "Default.hpp"
#include "Chunk.hpp"
#include "Types.hpp"

void Default::GenerateChunk(Chunk& chunk, Vector2i pos) {
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        int worldX = x + CHUNK_WIDTH * pos.x;
        int surface = 150 + 20.f * m_perlin.octave2D_01(worldX * 0.01, 0, 4);
        int ground = m_perlin.octave2D_01(worldX * 0.1, 0, 2) * 8;
        float roseNoise = m_perlin.octave2D_01(x * 0.3, 0, 1);

        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            int worldY = y + CHUNK_HEIGHT * pos.y;

            if (worldY > surface) {
                if (worldY < surface + 10 + ground) {
                    chunk.SetBlock(x, y, BLOCK_ID_DIRT);
                } else {
                    float cave = m_perlin.octave2D_01(worldX * 0.05, worldY * 0.05, 3);

                    chunk.SetBlock(x, y, (cave > worldY / 1000.f) ? BLOCK_ID_STONE : BLOCK_ID_AIR);
                }
            } else if (worldY == surface) {
                chunk.SetBlock(x, y, BLOCK_ID_GRASS);

                if (roseNoise > 0.7f && y > 0) {
                    chunk.SetBlock(x, y - 1, BLOCK_ID_ROSE);
                }
            }
        }
    }
}