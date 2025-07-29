#pragma once

#include "Blocks.hpp"
#include "Utils.hpp"
#include <array>
#include <cstdint>
#include "Types.hpp"

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 32
#define CHUNK_DEPTH 2

#define CHUNK_BACKGROUND_LAYER 0
#define CHUNK_FOREGROUND_LAYER 1

#define CHUNK_INDEX(x, y, z) INDEX_3D(x, y, z, CHUNK_WIDTH, CHUNK_HEIGHT)

class World;

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
    void SetBlock(int x, int y, int z, BlockID blockId);

    /**
     * @brief Get block at (x, y)
     * 
     * @param x local x coordinate of block [0; CHUNK_WIDTH]
     * @param y local y coordinate of block [0; CHUNK_HEIGHT]
     * @return BlockID 
     */
    BlockID GetBlock(int x, int y, int z) noexcept;

    void Fill(BlockID blockId);

    bool IsLightDirty() const noexcept { return m_dirtyLightning; }
    void SetLightDirty(bool dirty = true) { m_dirtyLightning = dirty; }

    void SetLight(int x, int y, uint8_t level);
    uint8_t GetLight(int x, int y) const noexcept;

    void UpdateLighting(World& world, const Vector2i& pos);

private:
    std::array<BlockID, CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH> m_blocks;
    std::array<uint8_t, CHUNK_WIDTH * CHUNK_HEIGHT> m_lightmap;
    bool m_dirtyLightning;
};