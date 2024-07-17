#pragma once
#include <SerializedObject.hpp>
#include <raylib.h>

#define BLOCK_SIZE_PIXELS 32
#define BS BLOCK_SIZE_PIXELS

class Block : public SerializedObject {
public:
    enum class BlockType : unsigned char {
        AIR = 0,
        GRASS,
        DIRT,
        STONE,
        COBLESTONE,
        PLANKS
    };
protected:
    static inline size_t m_size = 0;
    BlockType m_type;
    int m_x;
    int m_y;
    uint8_t m_layer;
    
public:
    Block(BlockType type);
    Block(BlockType type, int x, int y, uint8_t layer);

    BlockType getType() const;
    Vector2 getPosition() const;
    uint8_t getLayer() const;
    static size_t const getSize();

    virtual void update();

    void setPosition(int x, int y, uint8_t layer);

    ByteVector& serialize();
    int deserialize(ByteVector& bytes);
};