#pragma once
#include <Item.hpp>
#include <Types.hpp>
#include <cstddef>
#include <inventory/InventoryItem.hpp>

class World;

class Block : public Item {
protected:
    std::mutex m_mutex;
    MaterialType m_materialType;

public:
    std::shared_ptr<Block> static create(ItemID id);
    std::shared_ptr<Block> static create(Item& block);

    Block(ItemID id = ItemID::AIR);
    Block(Block& block);
    Block(InventoryItem& item);

    virtual void onTick(World* world, BlockPosition position) {}
    virtual void onRandomTick(World* world, BlockPosition position) {}

    virtual std::shared_ptr<InventoryItem> dropItem(std::shared_ptr<Item> tool);
};