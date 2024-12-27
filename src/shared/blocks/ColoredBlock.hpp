#pragma once
#include <Block.hpp>
#include <Types.hpp>
#include <SerializedObject.hpp>

class ColoredBlock : public Block {
private:
    Col3u m_color;
public:
    ColoredBlock(ID type, uint8_t subID = 0, int32_t x = 0, int32_t y = 0, uint8_t layer = 1);
    Col3u getColor();
};