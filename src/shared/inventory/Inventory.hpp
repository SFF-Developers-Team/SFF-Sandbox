#pragma once
#include <Types.hpp>
#include <Item.hpp>

struct InventorySlot {
    std::unique_ptr<Item> item;
    int count;
};

class Inventory {
protected:
    std::vector<InventorySlot> m_inventory;
    uint8_t m_selected;

public:
    Inventory(int slots);

    bool addItem(std::unique_ptr<Item> item, int count = 1);
    InventorySlot& getItem(int i) { return m_inventory[i]; }
    void swapItems(uint16_t a, uint16_t b) { std::swap(m_inventory[a], m_inventory[b]); }

    int getSlotsCount() { return m_inventory.size(); }

    // I DO NOT recommend to use this 
    auto& getInventory() { return m_inventory; }

    void setSelectedItem(uint8_t selected) { m_selected = selected % 9; }
    uint8_t getSelectedItem() { return m_selected; }

    InventorySlot* findItem(ItemID id);
    InventorySlot* findItem(ItemID id, int aux);

    void deleteItem(ItemID id, int count = 1);
    void gc();
};