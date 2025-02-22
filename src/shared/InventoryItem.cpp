#include <InventoryItem.hpp>
#include <world/Block.hpp>

InventoryItem::InventoryItem(InventoryItemType type, uint8_t id, uint8_t count) : ItemBase(id), m_type(type), m_count(count) {}
InventoryItem::InventoryItem(std::shared_ptr<ItemBase> item, uint8_t count) : InventoryItem(INVENTORY_TYPE_BLOCK, item->getID(), count) {}

bool InventoryItem::operator==(std::shared_ptr<InventoryItem> other) {
    return m_type == other->getType() && m_id == other->getID() && m_tags == other->getTags();
}

bool InventoryItem::operator==(std::shared_ptr<ItemBase> other) {
    return m_id == other->getID() && m_tags == other->getTags();
}