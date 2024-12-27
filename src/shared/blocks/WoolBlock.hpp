#pragma once
#include <ColoredBlock.hpp>

class WoolBlock : public ColoredBlock {
public:
    WoolBlock(uint8_t subID = 0, int32_t x = 0, int32_t y = 0, uint8_t layer = 1);

    std::shared_ptr<Block> getBlockForSubID(uint8_t subID) override;
};