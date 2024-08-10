#pragma once

#include <Block.hpp>

class World;
class Block;

class WorldGen {
protected:
    World* m_world;
    
public:
    WorldGen(World* world);

    virtual std::unique_ptr<Block> generateBlock(int x, int y, uint8_t layer) = 0;
};