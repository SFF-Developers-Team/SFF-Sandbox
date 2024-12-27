#include <WoolBlock.hpp>

WoolBlock::WoolBlock(uint8_t subID, int32_t x, int32_t y, uint8_t layer) : ColoredBlock(Block::ID::WOOL, subID, x, y, layer) {}

std::shared_ptr<Block> WoolBlock::getBlockForSubID(uint8_t subID) {
    return std::make_shared<WoolBlock>(subID);
}