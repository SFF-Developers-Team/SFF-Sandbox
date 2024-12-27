#include <Block.hpp>
#include <SerializedObject.hpp>

Block::Block(ID id, uint8_t subID, int32_t x, int32_t y, uint8_t layer) : m_id(id), m_x(x), m_y(y), m_layer(layer) {
    m_header = Header::BLOCK;
}

Block::Block(Block& block) : Block(block.m_id, block.m_subID, block.m_x, block.m_y, block.m_layer) {}

void Block::update() {}

void Block::setPos(int32_t x, int32_t y, uint8_t layer) {
    m_x = x;
    m_y = y;
    m_layer = layer;
}

Block::Type const Block::getTypeByID(Block::ID id) {
    switch(id) {
        case Block::ID::AIR: return Block::Type::DECORATION;
        case Block::ID::WOOL: return Block::Type::COLORED;
        default: return Block::Type::SOLID;
    }
}

std::vector<uint8_t>& Block::serialize() {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::serialize();

    addBytes(m_id);
    addBytes(m_subID);
    addBytes(m_x);
    addBytes(m_y);
    addBytes(m_layer);

    return m_bytes;
}

size_t Block::deserialize(std::vector<uint8_t>& bytes) {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::deserialize(bytes);

    m_id = getBytes<ID>(ID::AIR);
    m_x = getBytes<int32_t>(-1);
    m_y = getBytes<int32_t>(-1);
    m_layer = getBytes<uint8_t>(1);

    return m_offset;
}

std::size_t const Block::getSize() {
    return sizeof(Header) + sizeof(ID) + sizeof(uint8_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(uint8_t);
}

Rectf Block::getHitbox() {
    if(m_layer == 0 || getType() == Type::DECORATION) return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
    return Rectf {static_cast<float>(m_x), static_cast<float>(m_y), 1.0f, 1.0f};
}