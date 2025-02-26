#include <ItemBase.hpp>
#include <world/Block.hpp>

ItemBase::ItemBase(uint8_t id) : m_id(id) {}

ItemBase::ItemBase(ItemBase& itemPointer) : m_id(itemPointer.m_id), m_tags(itemPointer.m_tags) {}
