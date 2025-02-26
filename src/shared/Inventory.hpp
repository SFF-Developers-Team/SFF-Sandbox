#pragma once
#include <Types.hpp>
#include <InventoryItem.hpp>

class Inventory {
protected:
    std::vector<std::shared_ptr<InventoryItem>> m_inventory;
    uint8_t m_selected;

public:
    Inventory(uint16_t slots);

    bool addItem(std::shared_ptr<InventoryItem> item);
    std::shared_ptr<InventoryItem> getItem(uint16_t i) { return m_inventory[i]; }
    void setItem(uint16_t i, std::shared_ptr<InventoryItem> item) { m_inventory[i] = item; }
    void swapItems(uint16_t a, uint16_t b) { std::swap(m_inventory[a], m_inventory[b]); }

    int getSlotsCount() { return m_inventory.size(); }

    // I DO NOT recommend to use this 
    auto& getInventory() { return m_inventory; }

    void setSelectedItem(uint8_t selected) { m_selected = selected % 9; }
    uint8_t getSelectedItem() { return m_selected; }
};