#pragma once

#include "Sprite.hpp"
#include <memory>
#include <SerializedObject.hpp>

#define BLOCK_SIZE_PIXELS 32

class Block : public SerializedObject {
public:
    enum class BlockType : unsigned char {
        AIR = 0,
        GRASS,
        DIRT,
        STONE
    };
protected:
    Sprite* m_sprite;
    BlockType m_type;
    float m_durability; // Прочность блока
    int m_x;
    int m_y;
public:
    Block(BlockType type);
    Block(SObject &s);

    Sprite* getSprite() const;
    BlockType getType() const;

    virtual void update(); // error: undefined symbol: public: virtual void __cdecl Block::update(void)

    // multiplayer stuff
    SObject encodeObject() override; // undefined symbol: public: virtual class std::vector<unsigned char, class std::allocator<unsigned char>> __cdecl Block::encodeObject(void)
    void decodeObject(SObject &s) override {};
};