#pragma once

#include <world/Chunk.hpp>
#include <raylib.h>

class ClientChunk : public Chunk {
protected:
    RenderTexture2D m_render;
    uint16_t m_blockCount;

public:
    ClientChunk();
    ClientChunk(Chunk&& chunk);
    ~ClientChunk();

    auto& getPreRender() { return m_render; }
    void updateRender();

    void setBlock(BlockPosition pos, std::shared_ptr<Block> block) override;

    void draw(Vec2i pos);

    // for debug
    int getBlockCount() { return m_blockCount; }
};