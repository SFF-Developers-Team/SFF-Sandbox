#pragma once
#include <Item.hpp>

class Block;
class Item;
class Inventory;

class InventoryItem : public Item {
protected:
    int16_t m_count;
    Inventory* m_listener;

public:
    InventoryItem(ItemID id, int16_t count = 1);
    InventoryItem(Block& block, int16_t count = 1);
    InventoryItem(Item& item, int16_t count = 1);

    int16_t getCount() { return m_count; }
    void setCount(int16_t count) { m_count = count; }
    
    int16_t add(int amount);
    void sub(int amount);

    int16_t operator+=(int amount) { return add(amount); }
    void operator-=(int amount) { sub(amount); }

    void setListener(Inventory* inv) { m_listener = inv; }
};