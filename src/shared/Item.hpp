#pragma once
#include <Serializable.hpp>
#include <Types.hpp>

// TODO: ItemManager
#include <ItemDatabase.hpp>

class Block;

class Item : public Serializable {
protected:
    ItemID m_id;
    ItemTags m_tags;

public:
    Item(ItemID id);
    Item(Item& item);

    ItemID getID() { return m_id; }
    void setID(ItemID id) { m_id = id; }

    void setTag(TagID key, TagValue value) { m_tags[key] = value; }
    bool hasTag(TagID key) { return m_tags.contains(key); }
    void removeTag(TagID key) { m_tags.erase(key); }

    template <typename T>
    T getTag(TagID key) { return std::get<T>(m_tags[key]); }
    auto& getTags() { return m_tags; }
    void setTags(ItemTags const& newTags) { m_tags = newTags; }

    ItemType getType() { return gItemDatabase[m_id].type; }
    ItemType getBestTool() { return gItemDatabase[m_id].bestTool; }
    MaterialType getBestToolMaterial() { return gItemDatabase[m_id].bestToolMaterial; }
    std::string getName() { return gItemDatabase[m_id].name; }
    MaterialType getMaterial() { return gItemDatabase[m_id].material; }
    uint16_t getSpriteID() { return gItemDatabase[m_id].spriteIndex; }
    float getDurability() { return gItemDatabase[m_id].durability; }
    float getToolSpeed();
    bool isTool();
    int getMaxCount();

    DataStream serialize();
    bool deserialize(DataStream& bytes);

    bool operator==(Item const& other) const { return m_id == other.m_id && m_tags == other.m_tags; }
};