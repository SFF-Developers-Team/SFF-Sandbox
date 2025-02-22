#pragma once
#include <SerializedObject.hpp>
#include <Types.hpp>

class Block;

class ItemBase {
protected:
    ItemTags m_tags;
    uint8_t m_id;

public:
    ItemBase(uint8_t id);

    uint8_t getID() { return m_id; }
    void setID(uint8_t id) { m_id = id; }

    void setTag(TagID key, TagValue value) { m_tags[key] = value; }
    bool hasTag(TagID key) { return m_tags.contains(key); }
    void removeTag(TagID key) { m_tags.erase(key); }

    template <typename T>
    T getTag(TagID key) { return std::get<T>(m_tags[key]); }
    auto& getTags() { return m_tags; }

    bool operator==(ItemBase const& other);
};