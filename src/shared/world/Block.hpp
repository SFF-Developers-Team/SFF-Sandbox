#pragma once
#include <SerializedObject.hpp>
#include <Item.hpp>
#include <Types.hpp>
#include <cstddef>
#include <inventory/InventoryItem.hpp>

class World;

class Block : public Item {
    friend class World;
    friend class Chunk;

protected:
    int32_t m_x;
    int32_t m_y;
    uint8_t m_layer;

    std::mutex m_mutex;
    MaterialType m_materialType;

    World* m_world;

public:
    std::shared_ptr<Block> static create(ItemID id);
    std::shared_ptr<Block> static create(Item& block);
    std::shared_ptr<Block> static create(SerializedObject& obj);

    Block(ItemID id = ItemID::AIR);
    Block(Block& block);
    Block(InventoryItem& item);

    Rectf getHitbox();

    virtual void onTick() {}
    virtual void onRandomTick() {}

    void setPos(int32_t x, int32_t y, uint8_t layer);

    ByteVector serialize() override;
    size_t deserialize(ByteVector const& bytes) override;

    auto const getPos() { return Vec2i {m_x, m_y}; }
    auto const getLayer() { return m_layer; }

    float getDurability();

    virtual std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool);
};