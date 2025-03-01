#include <inventory/Inventory.hpp>

Inventory::Inventory(uint16_t slots) : m_inventory(slots), m_selected(0) {}

bool Inventory::addItem(std::shared_ptr<InventoryItem> item) {
    // Search for similar items and combine them
    for (auto& slot : m_inventory) {
        if (slot != nullptr && *slot == *item) {
            item->sub(slot->add(item->getCount()));
        }

        if (item->getCount() <= 0) {
            return true;
        }
    }

    // if selected slot is empty, put the items in it
    if (m_inventory[m_selected] == nullptr) {
        m_inventory[m_selected] = std::make_shared<InventoryItem>(*item);
        m_inventory[m_selected]->setCount(0);
        
        item->sub(m_inventory[m_selected]->add(item->getCount()));

        if (item->getCount() <= 0) {
            return true;
        }
    }

    // put other items into empty slots
    for (auto& slot : m_inventory) {
        if (slot == nullptr) {  
            slot = std::make_shared<InventoryItem>(*item);
            slot->setCount(0);

            item->sub(slot->add(item->getCount()));

            if (item->getCount() <= 0) {
                return true;
            }
        }
    }

    return false;
}