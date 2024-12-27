#pragma once
#include <SerializedObject.hpp>
#include <Types.hpp>
#include <cstddef>

class Block : public SerializedObject {
public:
    enum Type : uint8_t {
        DECORATION = 0,
        SOLID,
        COLORED
    };

    enum ID : uint8_t {
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
    ID m_id;
    uint8_t m_subID;
    int32_t m_x;
    int32_t m_y;
    uint8_t m_layer;
    
public:
    Block(ID id = ID::AIR, uint8_t subID = 0, int32_t x = 0, int32_t y = 0, uint8_t layer = 1);
    Block(Block& block);

    static std::size_t const getSize();
    static Type const getTypeByID(ID id);
    Rectf getHitbox();

    virtual void update();

    void setPos(int32_t x, int32_t y, uint8_t layer);
    void setID(ID id) { m_id = id; }

    ByteVector& serialize() override;
    size_t deserialize(ByteVector& bytes) override;

    static uint8_t getSubIDCount() { return 0; }
    virtual std::shared_ptr<Block> getBlockForSubID(uint8_t subID) { return std::shared_ptr<Block>(this); }

    auto const getID() { return m_id; }
    auto const getSubID() { return m_subID; }
    auto const getPos() { return Vec2i {m_x, m_y}; }
    auto const getLayer() { return m_layer; };
    auto const getType() { return getTypeByID(m_id); }
};