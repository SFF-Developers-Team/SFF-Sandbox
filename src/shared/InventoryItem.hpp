#pragma once
#include <ItemBase.hpp>

class InventoryItem : public ItemBase {
protected:
    InventoryItemType m_type;
    uint8_t m_count;

public:
    InventoryItem(InventoryItemType type, uint8_t id, uint8_t count = 1);
    InventoryItem(std::shared_ptr<ItemBase> item, uint8_t count = 1);

    uint8_t getCount() { return m_count; }
    void setCount(uint8_t count) { m_count = count; }
    void incrementCount() { m_count++; }
    void decrementCount() { m_count--; }

    bool isBlock() { return m_type == INVENTORY_TYPE_BLOCK; }

    InventoryItemType getType() { return m_type; }

    bool operator==(std::shared_ptr<InventoryItem> other);
    bool operator==(std::shared_ptr<ItemBase> other);
};