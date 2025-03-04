#include <Item.hpp>
#include <ItemDatabase.hpp>
#include <world/Block.hpp>

Item::Item(ItemID id) : m_id(id) {}

Item::Item(Item& itemPointer) : m_id(itemPointer.m_id), m_tags(itemPointer.m_tags) {}

ItemType Item::getType() {
    return gItemDatabase[m_id].type;
}

std::string Item::getName() {
    return gItemDatabase[m_id].name;
}

MaterialType Item::getMaterial() {
    return gItemDatabase[m_id].material;
}

uint16_t Item::getSpriteIndex() {
    return gItemDatabase[m_id].spriteIndex;
}

uint16_t Item::getMaxCount() {
    if (std::find(gTools.begin(), gTools.end(), getType()) != gTools.end()) {
        return 1;
    }

    return 64;
}