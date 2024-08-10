#pragma once
#include <SerializedObject.hpp>
#include <Types.hpp>
#include <cstddef>

#define BLOCK_SIZE_PIXELS 32.f
#define BS BLOCK_SIZE_PIXELS

class Block : public SerializedObject {
public:
    enum class BlockType : unsigned char {
        AIR = 0,
        GRASS,
        DIRT,
        STONE,
        COBLESTONE,
        PLANKS,
        WOOL,
        BEDROCK
    };
protected:
    static inline std::size_t m_size = 0;
    BlockType m_type;
    int m_x;
    int m_y;
    uint8_t m_layer;
    
public:
    Block(BlockType type);
    Block(BlockType type, int x, int y, uint8_t layer);

    BlockType getType() const;
    Vec2i getPosition() const;
    uint8_t getLayer() const;
    static std::size_t const getSize();
    Rectf getHitbox();

    virtual void update();

    void setPosition(int x, int y, uint8_t layer);
    void setType(BlockType type) { m_type = type; }

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);
};