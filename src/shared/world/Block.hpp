#pragma once
#include <SerializedObject.hpp>
#include <ItemBase.hpp>
#include <Types.hpp>
#include <cstddef>

class Block : public ItemBase, public SerializedObject {
protected:
    int32_t m_x;
    int32_t m_y;
    uint8_t m_layer;

    std::mutex m_mutex;
    MaterialType m_materialType;

public:
    static std::string const idToString(BlockID id);

    Block(BlockID id = BlockID::AIR, int32_t x = 0, int32_t y = 0, uint8_t layer = 1);
    Block(Block& block);
    Block(ItemBase& item);

    Rectf getHitbox();

    virtual void onTick() {}
    virtual void onRandomTick() {}

    void setPos(int32_t x, int32_t y, uint8_t layer);
    void setID(BlockID id) { m_id = id; }
    BlockID getID() { return static_cast<BlockID>(m_id); }

    ByteVector serialize() override;
    size_t deserialize(ByteVector const& bytes) override;

    auto const getPos() { return Vec2i {m_x, m_y}; }
    auto const getLayer() { return m_layer; };

    float getDurability();
};