#include "Types.hpp"
#include <Item.hpp>
#include <ItemDatabase.hpp>
#include <world/Block.hpp>

Item::Item(ItemID id) : Serializable(ObjectHeader::ITEM), m_id(id) {}

Item::Item(Item& item) : Item(item.m_id) {
    m_tags = item.m_tags;
}

int Item::getMaxCount() {
    return (isTool()) ? 1 : 64;
}

DataStream Item::serialize() {
    auto ret = Serializable::serialize();

    ret.add(m_id);
    ret.add<uint8_t>('T');
    ret.add<uint8_t>(m_tags.size());

    for(auto& [key, value] : m_tags) {
        ret.add<TagID>(key);

        switch(key) {
            case TAG_COLOR: ret.add<Col3u>(std::get<Col3u>(value)); break;
            case TAG_NATURAL:
            case TAG_GHOST: ret.add<bool>(std::get<bool>(value)); break;
        }
    }

    return ret;
}

bool Item::deserialize(DataStream& stream) {
    if(!Serializable::deserialize(stream)) return false;
    
    m_id = stream.get<ItemID>();

    if(stream.get<uint8_t>() == 'T') {
        auto tagsc = stream.get<uint8_t>();

        for(auto i = 0; i < tagsc; i++) {
            TagID key = stream.get<TagID>();

            switch(key) {
                case TAG_COLOR: m_tags[key] = stream.get<Col3u>({255, 255, 255}); break;
                case TAG_GHOST: m_tags[key] = stream.get<bool>(); break;
            }
        }
    }

    return true;
}

bool Item::isTool() {
    auto type = getType();
    return type > TYPE_ITEM;
}

float Item::getToolSpeed() {
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