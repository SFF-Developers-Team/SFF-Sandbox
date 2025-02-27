#include <InventoryItem.hpp>
#include <Item.hpp>

InventoryItem::InventoryItem(InventoryItemType type, uint16_t id, int16_t count) : ItemBase(id), m_type(type), m_count(count) {}

InventoryItem::InventoryItem(Block& block, int16_t count) : ItemBase(reinterpret_cast<ItemBase&>(block)), m_type(INV_BLOCK), m_count(count) {}

InventoryItem::InventoryItem(Item& item, int16_t count)  : ItemBase(reinterpret_cast<ItemBase&>(item)), m_type(INV_ITEM), m_count(count) {}

int16_t InventoryItem::add(int amount) {
    int16_t add = std::min(amount, 64 - m_count);
    m_count += add;

    return add;
}

void InventoryItem::sub(int amount) {
    m_count = std::max(0, m_count - amount);
}

bool InventoryItem::operator==(InventoryItem const& other) const {
    return m_type == other.m_type && m_id == other.m_id && m_tags == other.m_tags;
}