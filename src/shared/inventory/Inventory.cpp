#include <inventory/Inventory.hpp>
#include <Item.hpp>
#include <vector>

Inventory::Inventory(int slots) : m_inventory(slots), m_selected(0) {}

bool Inventory::addItem(std::unique_ptr<Item> item, int count) {
    if (item == nullptr) {
        return false;
    }

    // TODO: Make it spread on other slots when overflow

    // Search for similar items and combine them
    for (auto& slot : m_inventory) {
        if (slot.item != nullptr && *slot.item == *item) {
            slot.count += count;

            return true;
        }
    }

    // if selected slot is empty, put the items in it
    if (m_inventory[m_selected].item == nullptr) {
        m_inventory[m_selected].item = std::move(item);
        m_inventory[m_selected].count = count;
    
        return true;
    }

    // put other items into empty slots
    for (auto& slot : m_inventory) {
        if (slot.item == nullptr) {  
            slot.item = std::move(item);
            slot.count = count;
        }
    }

    return false;
}

InventorySlot* Inventory::findItem(int id) {
    for (auto i = 0; i < m_inventory.size(); i++) {
        if (m_inventory[i].item != nullptr && m_inventory[i].item->getID() == id) {
            return &m_inventory[i];
        }
    }

    return nullptr;
}

InventorySlot* Inventory::findItem(int id, int aux) {
    for (auto i = 0; i < m_inventory.size(); i++) {
        if (m_inventory[i].item != nullptr && m_inventory[i].item->getID() == id && m_inventory[i].item->getAux() == aux) {
            return &m_inventory[i];
        }
    }

    return nullptr;
}

void Inventory::deleteItem(int id, int count) {
    auto slot = findItem(id);

    if (slot != nullptr) {
        slot->count -= count;
    }
}

void Inventory::gc() {
    for (auto& slot : m_inventory) {
        if (slot.count <= 0) {
            slot.item = nullptr;
            slot.count = 0;
        }
    }
}