#pragma once

#include <WorldGen.hpp>

class WorldGenRubyDung : public WorldGen {
public:
    WorldGenRubyDung(World* world);

    virtual Block* generateBlock(int x, int y) override;
};