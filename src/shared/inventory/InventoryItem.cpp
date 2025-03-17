#include <inventory/InventoryItem.hpp>
#include <Item.hpp>
#include <ItemDatabase.hpp>
#include <inventory/Inventory.hpp>

InventoryItem::InventoryItem(ItemID id, int16_t count) : Item(id), m_count(count) {
    if (gItemDatabase.find(id) != gItemDatabase.end()) {
        m_tags = gItemDatabase[id].defaultTags;
    }
}

InventoryItem::InventoryItem(Block& block, int16_t count) : InventoryItem(reinterpret_cast<Item&>(block), count) {}

InventoryItem::InventoryItem(Item& item, int16_t count)  : Item(reinterpret_cast<Item&>(item)), m_count(count) {
    if (hasTag(TAG_NATURAL)) {
        removeTag(TAG_NATURAL);
    }
}

int16_t InventoryItem::add(int amount) {
    int16_t add = std::min(amount, getMaxCount() - m_count);
    m_count += add;

    if (m_listener) m_listener->onItemChanged(this);

    return add;
}

void InventoryItem::sub(int amount) {
    m_count = std::max(0, m_count - amount);

    if (m_listener) m_listener->onItemChanged(this);
}