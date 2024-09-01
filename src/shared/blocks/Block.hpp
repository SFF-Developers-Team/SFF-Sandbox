#pragma once
#include <SerializedObject.hpp>
#include <Types.hpp>
#include <cstddef>

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
    BlockType m_type;
    int m_x;
    int m_y;
    uint8_t m_layer;
    
public:
    Block(BlockType type);
    Block(BlockType type, int x, int y, uint8_t layer);

    static std::size_t const getSize();
    BlockType const getType();
    Vec2i const getPosition();
    uint8_t const getLayer();
    Rectf getHitbox();

    virtual void update();

    void setPosition(int x, int y, uint8_t layer);
    void setType(BlockType type) { m_type = type; }

    ByteVector& serialize() override;
    size_t deserialize(ByteVector& bytes) override;
};