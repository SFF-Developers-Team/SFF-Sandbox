#include <Block.hpp>
#include <SerializedObject.hpp>
Block::Block(BlockType type) : m_type(type) {
    m_header = Header::BLOCK;
}

Block::Block(BlockType type, int x, int y, uint8_t layer) : m_type(type), m_x(x), m_y(y), m_layer(layer) {
    m_header = Header::BLOCK;
}

Block::BlockType Block::getType() const {
    return m_type;
}

Vec2i Block::getPosition() const {
    return Vec2i {m_x, m_y};
}

uint8_t Block::getLayer() const {
    return m_layer;
}

void Block::update() {}

void Block::setPosition(int x, int y, uint8_t layer) {
    m_x = x;
    m_y = y;
    m_layer = layer;
}

std::vector<uint8_t>& Block::serialize() {
    SerializedObject::serialize();

    addBytes(m_type);
    addBytes(m_x);
    addBytes(m_y);
    addBytes(m_layer);

    return m_bytes;
}

int Block::deserialize(std::vector<uint8_t>& bytes) {
    SerializedObject::deserialize(bytes);

    m_type = getBytes<BlockType>();
    m_x = getBytes<int>();
    m_y = getBytes<int>();
    m_layer = getBytes<unsigned char>(1);

    return m_offset;
}

std::size_t const Block::getSize() {
    if(!m_size) {
        // Testing block size
        auto block = new Block(Block::BlockType::AIR);
        auto bytes = block->serialize();
        
        m_size = bytes.size();
        
        delete block;
    }

    return m_size;
}

Rectf Block::getHitbox() {
    if(m_layer == 0 || m_type == BlockType::AIR) return Rectf {0.0f, 0.0f, 0.0f, 0.0f};
    return Rectf {(float)m_x, (float)m_y, 1.0f, 1.0f};
}