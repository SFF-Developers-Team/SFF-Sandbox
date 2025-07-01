#include "Flat.hpp"
#include "Chunk.hpp"
#include "Types.hpp"

void Flat::GenerateChunk(Chunk& chunk, Vector2i pos) {
    chunk.Fill((pos.y > 8) ? BLOCK_ID_DIRT : BLOCK_ID_AIR);

    if (pos.y == 9) {
        for (int x = 0; x < CHUNK_WIDTH; x++) {
            chunk.SetBlock(x, 0, CHUNK_FOREGROUND_LAYER, BLOCK_ID_GRASS);
            chunk.SetBlock(x, 0, CHUNK_BACKGROUND_LAYER, BLOCK_ID_GRASS);
        }
    }
}