#include <world/Block.hpp>
#include <SerializedObject.hpp>
#include <assert.h>

Block::Block(ID id, int32_t x, int32_t y, uint8_t layer) : m_id(id), m_x(x), m_y(y), m_layer(layer) {
    m_header = Header::BLOCK;
}

Block::Block(Block& block) : Block(block.m_id, block.m_x, block.m_y, block.m_layer) {
    m_tags = block.m_tags;
}

std::string const Block::idToString(ID id) {
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
        default: return "Unknown";
    }
}

void Block::update() {}

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
        add<Block::TagID>(key);

        switch(key) {
            case COLOR:
                add<Col3u>(std::get<Col3u>(value));
                break;
            case GHOST:
                add<bool>(std::get<bool>(value));
                break;
        }
    }

    return bytes();
}

size_t Block::deserialize(ByteVector const& bytes) {
    SerializedObject::deserialize(bytes);

    m_id = get<ID>();
    m_x = get<int32_t>();
    m_y = get<int32_t>();
    m_layer = get<uint8_t>(1);

    if(get<uint8_t>() == 0x20) {
        auto tagsc = get<uint16_t>();

        for(int i = 0; i < tagsc; i++) {
            TagID key = get<Block::TagID>();

            switch(key) {
                case COLOR:
                    m_tags[key] = get<Col3u>({255, 255, 255});
                    break;

                case GHOST:
                    m_tags[key] = get<bool>();
                    break;
            }
        }
    }

    return m_offset;
}

void Block::setTag(TagID key, TagValue value) {
    m_tags[key] = value;
}

bool Block::hasTag(TagID key) {
    return m_tags.contains(key);
}

void Block::removeTag(TagID key) {
    m_tags.erase(key);
}

Rectf Block::getHitbox() {
    if(m_layer == 0 || m_id == AIR || (hasTag(GHOST) && getTag<bool>(GHOST))) {
        return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
    }

    return Rectf {static_cast<float>(m_x), static_cast<float>(m_y), 1.0f, 1.0f};
}