#pragma once
#include <ItemBase.hpp>

class Block;
class Item;

class InventoryItem : public ItemBase {
protected:
    InventoryItemType m_type;
    int16_t m_count;

public:
    InventoryItem(InventoryItemType type, uint16_t id, int16_t count = 1);
    InventoryItem(Block& block, int16_t count = 1);
    InventoryItem(Item& item, int16_t count = 1);

    int16_t getCount() { return m_count; }
    void setCount(int16_t count) { m_count = count; }
    
    int16_t add(int amount);
    void sub(int amount);

    InventoryItemType getType() { return m_type; }

    int16_t operator+=(int amount) { return add(amount); }
    void operator-=(int amount) { sub(amount); }
    bool operator==(InventoryItem const& other) const;
};