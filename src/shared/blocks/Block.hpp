#pragma once
#include <SerializedObject.hpp>
#include <Types.hpp>
#include <cstddef>

class Block : public SerializedObject {
public:
    enum class Type : unsigned char {
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
    Type m_type;
    int m_x;
    int m_y;
    uint8_t m_layer;
    
public:
    Block(Type type);
    Block(Type type, int x, int y, uint8_t layer);

    static std::size_t const getSize();
    Type const getType();
    Vec2i const getPosition();
    uint8_t const getLayer();
    Rectf getHitbox();

    virtual void update();

    void setPosition(int x, int y, uint8_t layer);
    void setType(Type type) { m_type = type; }

    ByteVector& serialize() override;
    size_t deserialize(ByteVector& bytes) override;
};