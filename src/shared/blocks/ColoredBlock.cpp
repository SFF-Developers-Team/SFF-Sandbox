#include <ColoredBlock.hpp>

ColoredBlock::ColoredBlock(ID type, uint8_t subID, int32_t x, int32_t y, uint8_t layer) : Block(type, subID, x, y, layer) {
    if(m_subID > 13) m_subID = 0;
}

Col3u ColoredBlock::getColor() {
    switch(m_subID) {
        default:
        case 0: return {255, 255, 255};
        case 1: return {255, 0, 0};
        case 2: return {255, 128, 0};
        case 3: return {255, 255, 0};
        case 4: return {0, 255, 0};
        case 5: return {0, 255, 255};
        case 6: return {0, 0, 255};
        case 7: return {128, 0, 255};
        case 8: return {255, 0, 255};
        case 9: return {255, 128, 255};
        case 10: return {255, 128, 255};
        case 11: return {128, 128, 128};
        case 12: return {0, 0, 0};
        case 13: return {128, 64, 0};
    }
}