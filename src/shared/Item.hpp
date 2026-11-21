#pragma once
#include <Serializable.hpp>
#include <Types.hpp>

// TODO: ItemManager
#include <ItemDatabase.hpp>

class Block;

class Item : public Serializable {
protected:
    int m_id;
    int m_aux;

public:
    Item(int id) : Serializable(ObjectHeader::ITEM), m_id(id), m_aux(0) {}
    Item(ItemID id) : Item(static_cast<int>(id)) {}
    Item(BlockID id) : Item(static_cast<int>(id)) {}

    int getID() const { return m_id; }
    int getAux() const {return m_aux; }
    // void setID(int id) { m_id = id; }

    bool isBlockItem() const { return m_id < ITEM_ID_BEGIN; }

    ItemType getType() const { return gItemDatabase[(ItemID)m_id].type; }
    ItemType getBestTool() const { return gItemDatabase[(ItemID)m_id].bestTool; }
    MaterialType getBestToolMaterial() const { return gItemDatabase[(ItemID)m_id].bestToolMaterial; }
    std::string_view getName() const { return gItemDatabase[(ItemID)m_id].name; }
    MaterialType getMaterial() const { return gItemDatabase[(ItemID)m_id].material; }
    uint16_t getSpriteID() const { return gItemDatabase[(ItemID)m_id].spriteIndex; }
    float getDurability() const { return gItemDatabase[(ItemID)m_id].durability; }

    float getToolSpeed() const;
    bool isTool() const;
    int getMaxCount() const;

    DataStream serialize();
    bool deserialize(DataStream& bytes);

    bool operator==(Item const& other) const { return m_id == other.m_id && m_aux == other.m_aux; }
};