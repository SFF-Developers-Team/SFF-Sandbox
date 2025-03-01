#pragma once
#include <SerializedObject.hpp>
#include <Item.hpp>
#include <Types.hpp>
#include <cstddef>
#include <inventory/InventoryItem.hpp>

class Block : public Item {
protected:
    int32_t m_x;
    int32_t m_y;
    uint8_t m_layer;

    std::mutex m_mutex;
    MaterialType m_materialType;

public:

    Block(ItemID id = ItemID::AIR, int32_t x = 0, int32_t y = 0, uint8_t layer = 1);
    Block(Block& block);
    Block(InventoryItem& item);

    Rectf getHitbox();

    virtual void onTick() {}
    virtual void onRandomTick() {}

    void setPos(int32_t x, int32_t y, uint8_t layer);

    ByteVector serialize() override;
    size_t deserialize(ByteVector const& bytes) override;

    auto const getPos() { return Vec2i {m_x, m_y}; }
    auto const getLayer() { return m_layer; };

    float getDurability();

    std::shared_ptr<InventoryItem> dropItem();
};