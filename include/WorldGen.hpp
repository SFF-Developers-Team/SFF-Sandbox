#pragma once

#include <Block.hpp>

class World;

class WorldGen {
protected:
    World *m_world;
public:
    virtual Block::BlockType generateBlock(int x, int y) = 0;
    virtual void generateWorld(World *world);
};