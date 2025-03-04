#include <inventory/InventoryItem.hpp>
#include <Item.hpp>
#include <ItemDatabase.hpp>

InventoryItem::InventoryItem(ItemID id, int16_t count) : Item(id), m_count(count) {
    if (gItemDatabase.find(id) != gItemDatabase.end()) {
        m_tags = gItemDatabase[id].defaultTags;
    }
}

InventoryItem::InventoryItem(Block& block, int16_t count) : Item(reinterpret_cast<Item&>(block)), m_count(count) {}

InventoryItem::InventoryItem(Item& item, int16_t count)  : Item(reinterpret_cast<Item&>(item)), m_count(count) {}

int16_t InventoryItem::add(int amount) {
    int16_t add = std::min(amount, getMaxCount() - m_count);
    m_count += add;

    return add;
}

void InventoryItem::sub(int amount) {
    m_count = std::max(0, m_count - amount);
}