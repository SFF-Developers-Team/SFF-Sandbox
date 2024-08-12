#pragma once

#include "Sprite.hpp"
#include <memory>
#include <SerializedObject.hpp>
#include <array>

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

    Sprite* getSprite() const;
    BlockType getType() const;

    virtual void update();

    void setPosition(int x, int y);

    // multiplayer stuff
    SObject encodeObject() override;
    int decodeObject(SObject &s) override;

    std::array<int, 2> getPosition();
};