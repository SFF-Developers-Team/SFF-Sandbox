#pragma once

#include <WorldGen.hpp>

class WorldGenRubyDung : public WorldGen {
public:
    WorldGenRubyDung(World* world);

    virtual std::unique_ptr<Block> generateBlock(int x, int y, uint8_t layer) override;
};