#include <ItemBase.hpp>
#include <world/Block.hpp>

ItemBase::ItemBase(uint8_t id) : m_id(id) {}

bool ItemBase::operator==(ItemBase const& other) {
    return m_id == other.m_id && m_tags == other.m_tags;
}