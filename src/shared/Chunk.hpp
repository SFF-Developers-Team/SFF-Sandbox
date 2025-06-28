#pragma once

#include "Types.hpp"
#include "Utils.hpp"
#include <array>

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 32
#define CHUNK_INDEX(x, y) INDEX_2D(x, y, CHUNK_WIDTH)

class Chunk {
public:
    /**
     * @brief Construct a new empty Chunk object
     */
    Chunk();

    /**
     * @brief Set block at (x, y)
     *
     * @param x local x coordinate of block [0; 32]
     * @param y local y coordinate of block [0; 32]
     * @param blockId id of the block
     */
    void SetBlock(int x, int y, BlockID blockId);

    /**
     * @brief Get block at (x, y)
     * 
     * @param x local x coordinate of block [0; 32]
     * @param y local y coordinate of block [0; 32]
     * @return BlockID 
     */
    BlockID GetBlock(int x, int y);

    void Fill(BlockID blockId);

#ifdef SANDBOX_CLIENT
    void Draw();
#endif

private:
    std::array<BlockID, CHUNK_WIDTH * CHUNK_HEIGHT> m_blocks;
};