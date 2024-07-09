#include "Block.hpp"
#include "GenericTools.hpp"

Block::Block(BlockType type) : m_type(type), m_sprite(Sprite::get((int)type)) {

}

Block::Block(SObject &s) {
    decodeObject(s);
}

Sprite* Block::getSprite() const {
    return m_sprite;
}

Block::BlockType Block::getType() const {
    return m_type;
}

SerializedObject::SObject Block::encodeObject() {
    SObject vec = {
        (unsigned char)m_type
    };

    GenericTools::addVectors<unsigned char>(
        &vec, GenericTools::valueToVector<BlockType>(m_type)
    );
    GenericTools::addVectors<unsigned char>(
        &vec, GenericTools::valueToVector<int>(m_x)
    );
    GenericTools::addVectors<unsigned char>(
        &vec, GenericTools::valueToVector<int>(m_y)
    );

    return vec;
}

void Block::update() {}