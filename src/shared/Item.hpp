#pragma once
#include <SerializedObject.hpp>
#include <Types.hpp>

class Block;

class Item : public SerializedObject {
protected:
    ItemID m_id;
    ItemTags m_tags;

public:
    Item(ItemID id);
    Item(Item& itemPointer);

    ItemID getID() { return m_id; }
    void setID(ItemID id) { m_id = id; }

    void setTag(TagID key, TagValue value) { m_tags[key] = value; }
    bool hasTag(TagID key) { return m_tags.contains(key); }
    void removeTag(TagID key) { m_tags.erase(key); }

    template <typename T>
    T getTag(TagID key) { return std::get<T>(m_tags[key]); }
    auto& getTags() { return m_tags; }

    ItemType getType();
    std::string getName();
    MaterialType getMaterial();
    uint16_t getSpriteIndex();

    uint16_t getMaxCount();

    bool operator==(Item const& other) const { return m_id == other.m_id && m_tags == other.m_tags; }
};