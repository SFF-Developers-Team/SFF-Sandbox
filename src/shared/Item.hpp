#pragma once
#include <Serializable.hpp>
#include <Types.hpp>

class Tile;

class Item : public Serializable {
protected:
    ItemID m_id;
    int m_aux;

public:
    Item(ItemID id) : Serializable(ObjectHeader::ITEM), m_id(id), m_aux(0) {}

    ItemID getID() const { return m_id; }
    int getAux() const {return m_aux; }

    static std::string_view GetName(ItemID id);
    static ItemType GetType(ItemID id);
    static ItemMaterial GetMaterial(ItemID id);
    static TileID GetTile(ItemID id);
    static int GetSpriteIndex(ItemID id);
    static bool IsTileItem(ItemID id) { return GetType(id) == ItemType::TYPE_TILE; }
    static float GetToolSpeed(ItemID id);

    DataStream serialize();
    bool deserialize(DataStream& bytes);

    bool operator==(Item const& other) const { return m_id == other.m_id && m_aux == other.m_aux; }
};