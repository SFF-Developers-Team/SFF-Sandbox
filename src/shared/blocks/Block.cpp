#include <Block.hpp>
#include <SerializedObject.hpp>

Block::Block(Type type) : m_type(type) {
    m_header = Header::BLOCK;
}

Block::Block(Type type, int x, int y, uint8_t layer) : m_type(type), m_x(x), m_y(y), m_layer(layer) {
    m_header = Header::BLOCK;
}

Block::Type const Block::getType() {
    return m_type;
}

Vec2i const Block::getPosition() {
    return Vec2i {m_x, m_y};
}

uint8_t const Block::getLayer() {
    return m_layer;
}

void Block::update() {}

void Block::setPosition(int x, int y, uint8_t layer) {
    m_x = x;
    m_y = y;
    m_layer = layer;
}

std::vector<uint8_t>& Block::serialize() {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::serialize();

    addBytes(m_type);
    addBytes(m_x);
    addBytes(m_y);
    addBytes(m_layer);

    return m_bytes;
}

size_t Block::deserialize(std::vector<uint8_t>& bytes) {
    std::lock_guard<std::mutex> guard(m_mutex);
    SerializedObject::deserialize(bytes);

    m_type = getBytes<Type>(Type::AIR);
    m_x = getBytes<int32_t>(-1);
    m_y = getBytes<int32_t>(-1);
    m_layer = getBytes<uint8_t>(1);

    return m_offset;
}

std::size_t const Block::getSize() {
    return sizeof(Header) + sizeof(Type) + sizeof(int32_t) + sizeof(int32_t) + sizeof(uint8_t);
}

Rectf Block::getHitbox() {
    if(m_layer == 0 || m_type == Type::AIR) return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
    return Rectf {(float)m_x, (float)m_y, 1.0f, 1.0f};
}