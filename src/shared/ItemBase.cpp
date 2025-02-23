#include <ItemBase.hpp>
#include <world/Block.hpp>

ItemBase::ItemBase(uint8_t id) : m_id(id) {}

ItemBase::ItemBase(std::shared_ptr<ItemBase> itemPointer) : m_id(itemPointer->getID()), m_tags(itemPointer->getTags()) {}

bool ItemBase::operator==(ItemBase const& other) {
    return m_id == other.m_id && m_tags == other.m_tags;
}