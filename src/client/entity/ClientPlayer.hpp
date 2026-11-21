#pragma once
#include <entity/Player.hpp>

class ClientPlayer : public Player {
public:
    ClientPlayer(World& world);

    void draw();
};