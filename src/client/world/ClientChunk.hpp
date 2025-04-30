#pragma once

#include "external/glfw/src/internal.h"
#include <world/Chunk.hpp>
#include <raylib.h>

class ClientChunk : public Chunk {
protected:
    GLuint m_vbo;
    GLuint m_vao;
    uint16_t m_blockCount;

public:
    ClientChunk();
    ClientChunk(Chunk&& chunk);

    void rebuild();

    void setBlock(BlockPosition pos, std::shared_ptr<Block> block) override;

    void draw(Vec2i pos);

    // for debug
    int getBlockCount() { return m_blockCount; }
};