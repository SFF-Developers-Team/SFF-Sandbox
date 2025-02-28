#include <Item.hpp>
#include <ItemDatabase.hpp>
#include <world/Block.hpp>

Item::Item(ItemID id) : m_id(id) {}

Item::Item(Item& itemPointer) : m_id(itemPointer.m_id), m_tags(itemPointer.m_tags) {}

ItemType Item::getType() {
    return gBlocksDatabase[m_id].type;
}

std::string Item::getName() {
    return gBlocksDatabase[m_id].name;
}

MaterialType Item::getMaterial() {
    return gBlocksDatabase[m_id].material;
}

uint16_t Item::getSpriteIndex() {
    return gBlocksDatabase[m_id].spriteIndex;
}