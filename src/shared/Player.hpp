#pragma once

#include "Entity.hpp"

class Player : public Entity {
public: 
    Player(World& world, RVector2 position) : Entity(world, position) {}

    virtual BlockPosition GetTargetBlock() { return {0, 0, 0}; }
protected:
    BlockPosition m_blockPos = {};
};