#pragma once

#include <Block.hpp>

class WorldGen {
public:
    virtual Block generateBlock(int x, int y);
};