#include <Block.hpp>
#include <SerializedObject.hpp>

Block::Block(ID id, int32_t x, int32_t y, uint8_t layer) : m_id(id), m_x(x), m_y(y), m_layer(layer) {
    m_header = Header::BLOCK;
}

Block::Block(Block& block) : Block(block.m_id, block.m_x, block.m_y, block.m_layer) {
    m_tags = block.m_tags;
}

void Block::update() {}

void Block::setPos(int32_t x, int32_t y, uint8_t layer) {
    m_x = x;
    m_y = y;
    m_layer = layer;
}

std::vector<uint8_t>& Block::serialize() {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::serialize();

    addBytes(m_id);
    addBytes(m_x);
    addBytes(m_y);
    addBytes(m_layer);

    return m_bytes;
}

size_t Block::deserialize(std::vector<uint8_t>& bytes) {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::deserialize(bytes);

    m_id = getBytes<ID>(ID::AIR);
    m_x = getBytes<int32_t>();
    m_y = getBytes<int32_t>();
    m_layer = getBytes<uint8_t>(1);

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
    if(m_layer == 0 || m_id == AIR) {
        return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
    }

    return Rectf {static_cast<float>(m_x), static_cast<float>(m_y), 1.0f, 1.0f};
}