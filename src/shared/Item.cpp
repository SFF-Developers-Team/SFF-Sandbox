#include "Types.hpp"
#include <Item.hpp>
#include <ItemDatabase.hpp>
#include <world/Block.hpp>

int Item::getMaxCount() const {
    return (isTool()) ? 1 : 64;
}

DataStream Item::serialize() {
    auto ret = Serializable::serialize();

    ret.add(m_id);
    ret.add(m_aux);

    return ret;
}

bool Item::deserialize(DataStream& stream) {
    if(!Serializable::deserialize(stream)) return false;
    
    m_id = stream.get<int>();
    m_aux = stream.get<int>();

    return true;
}

bool Item::isTool() const {
    auto type = getType();
    return type > TYPE_ITEM;
}

float Item::getToolSpeed() const {
    auto type = getType();

    if (type > TYPE_ITEM) {
        if (type == TYPE_WEAPON) {
            return 1.5f;
        }

        switch (getMaterial()) {
            case MATERIAL_WOOD: return 2.f;
            case MATERIAL_STONE: return 4.f;
            case MATERIAL_IRON: return 6.f;
            case MATERIAL_DIAMOND: return 8.f;
            case MATERIAL_GOLD: return 12.f;
            default: return 1.f;
        }
    }

    return 1.f;
}