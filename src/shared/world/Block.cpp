#include <world/Block.hpp>
#include <SerializedObject.hpp>
#include <cassert>

Block::Block(BlockID id, int32_t x, int32_t y, uint8_t layer) 
    : ItemBase(id), SerializedObject(Header::BLOCK), m_x(x), m_y(y), m_layer(layer) {

    switch(id) {
        case GRASS:
        case DIRT:
            m_materialType = MATERIAL_DIRT;
            break;
        case STONE:
        case COBLESTONE:
        case BRICKS:
        case COAL_ORE:
        case IRON_ORE:
        case GOLD_ORE:
        case DIAMOND_ORE:
        case LAPIZ_ORE:
        case FURHANCE:
            m_materialType = MATERIAL_STONE;
            break;
        case PLANKS:
        case OAK_LOG:
        case BOOKSHELF:
            m_materialType = MATERIAL_WOOD;
            break;
    };
}

Block::Block(Block& block) : Block(static_cast<BlockID>(block.m_id), block.m_x, block.m_y, block.m_layer) {
    m_tags = block.m_tags;
}

Block::Block(ItemBase& item) : Block(static_cast<BlockID>(item.getID())) {
    m_tags = item.getTags();
}

std::string const idToString(BlockID id) {
    switch(id) {
        case AIR: return "Air";
        case GRASS: return "Grass";
        case DIRT: return "Dirt";
        case STONE: return "Stone";
        case COBLESTONE: return "Coblestone";
        case PLANKS: return "Planks";
        case WOOL: return "Wool";
        case BEDROCK: return "Bedrock";
        case BRICKS: return "Bricks";
        case OAK_LOG: return "Oak log";
        case LEAVES: return "Leaves";
        case COAL_ORE: return "Coal ore";
        case IRON_ORE: return "Iron ore";
        case GOLD_ORE: return "Gold ore";
        case DIAMOND_ORE: return "Diamond ore";
        case LAPIZ_ORE: return "Lapiz ore";
        case BOOKSHELF: return "Bookshelf";
        case FLOWER_POT: return "Flower pot";
        case FURHANCE:
        case ACTIVE_FURHANCE: return "Furhance";
        default: return "Unknown";
    }
}

void Block::setPos(int32_t x, int32_t y, uint8_t layer) {
    m_x = x;
    m_y = y;
    m_layer = layer;
}

ByteVector Block::serialize() {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::serialize();

    add(m_id);
    add(m_x);
    add(m_y);
    add(m_layer);
    add<uint8_t>(0x20);
    add<uint16_t>(static_cast<uint16_t>(m_tags.size()));

    for(auto& [key, value] : m_tags) {
        add<TagID>(key);

        switch(key) {
            case TAG_COLOR: add<Col3u>(std::get<Col3u>(value)); break;
            case TAG_GHOST: add<bool>(std::get<bool>(value)); break;
        }
    }

    return bytes();
}

size_t Block::deserialize(ByteVector const& bytes) {
    SerializedObject::deserialize(bytes);

    m_id = get<BlockID>();
    m_x = get<int32_t>();
    m_y = get<int32_t>();
    m_layer = get<uint8_t>(1);

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

Rectf Block::getHitbox() {
    if(m_layer == 0 || m_id == AIR || (hasTag(TAG_GHOST) && getTag<bool>(TAG_GHOST))) {
        return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
    }

    return Rectf {static_cast<float>(m_x), static_cast<float>(m_y), 1.0f, 1.0f};
}

float Block::getDurability() {
    switch (m_materialType) {
        default:
        case MATERIAL_WOOL:
        case MATERIAL_DIRT: return 10.f;
        case MATERIAL_WOOD: return 25.f;
        case MATERIAL_STONE: return 50.f;
    }
}