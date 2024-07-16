#pragma once

#include <Block.hpp>

class World;
class Block;

class WorldGen {
protected:
    World* m_world;
public:
    WorldGen(World* world);

    virtual Block* generateBlock(int x, int y) = 0;
};