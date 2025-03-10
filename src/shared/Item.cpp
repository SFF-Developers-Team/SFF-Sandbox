#include <Item.hpp>
#include <ItemDatabase.hpp>
#include <world/Block.hpp>

Item::Item(ItemID id) : m_id(id) {}

Item::Item(Item& itemPointer) : m_id(itemPointer.m_id), m_tags(itemPointer.m_tags) {}

ItemType Item::getType() {
    return gItemDatabase[m_id].type;
}

std::string Item::getName() {
    return gItemDatabase[m_id].name;
}

MaterialType Item::getMaterial() {
    return gItemDatabase[m_id].material;
}

uint16_t Item::getSpriteIndex() {
    return gItemDatabase[m_id].spriteIndex;
}

uint16_t Item::getMaxCount() {
    if (std::find(gTools.begin(), gTools.end(), getType()) != gTools.end()) {
        return 1;
    }

    return 64;
}

ByteVector Item::serialize() {
    SerializedObject::serialize();

    add(m_id);
    add<uint8_t>(0x20);
    add<uint16_t>(m_tags.size());

    for(auto& [key, value] : m_tags) {
        add<TagID>(key);

        switch(key) {
            case TAG_COLOR: add<Col3u>(std::get<Col3u>(value)); break;
            case TAG_NATURAL:
            case TAG_GHOST: add<bool>(std::get<bool>(value)); break;
        }
    }

    return bytes();
}

size_t Item::deserialize(ByteVector const& bytes) {
    SerializedObject::deserialize(bytes);
    
    m_id = get<ItemID>();

    if(get<uint8_t>() == 0x20) {
        auto tagsc = get<uint16_t>();

        for(int i = 0; i < tagsc; i++) {
            TagID key = get<TagID>();

            switch(key) {
                case TAG_COLOR:
                    m_tags[key] = get<Col3u>({255, 255, 255});
                    break;

                case TAG_GHOST:
                    m_tags[key] = get<bool>();
                    break;
            }
        }
    }

    return m_offset;
}