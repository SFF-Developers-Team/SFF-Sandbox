#include "Block.hpp"
#include "GenericTools.hpp"

Block::Block(BlockType type) : m_type(type), m_sprite(Sprite::get((int)type)) {
    m_objectID = 0;
}

Sprite* Block::getSprite() const {
    return m_sprite;
}

Block::BlockType Block::getType() const {
    return m_type;
}

SerializedObject::SObject Block::encodeObject() {
    SObject vec = SerializedObject::encodeObject();

    GenericTools::addVectors<unsigned char>(
        &vec, GenericTools::valueToVector<BlockType>(&m_type)
    );
    GenericTools::addVectors<unsigned char>(
        &vec, GenericTools::valueToVector<int>(&m_x)
    );
    GenericTools::addVectors<unsigned char>(
        &vec, GenericTools::valueToVector<int>(&m_y)
    );

    return vec;
}

void Block::update() {}

void Block::setPosition(int x, int y) {
    m_x = x;
    m_y = y;
}

int Block::decodeObject(SObject &s) {
    unsigned int _offset = SerializedObject::decodeObject(s);
    unsigned int offset = _offset;
    unsigned int required = sizeof(BlockType) + sizeof(int) + sizeof(int);

    if (required > s.size() - offset) return s.size() - offset;

    m_type = GenericTools::vectorToValue<BlockType>(s, offset);
    offset += sizeof(BlockType);

    m_x = GenericTools::vectorToValue<int>(s, offset);
    offset += sizeof(int);
    
    m_y = GenericTools::vectorToValue<int>(s, offset);
    offset += sizeof(int);

    sizeof(SerializedObject);

    return s.size();
}

std::array<int, 2> Block::getPosition() {
    return {m_x, m_y};
}