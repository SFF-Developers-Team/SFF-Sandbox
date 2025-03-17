#pragma once
#include <entity/Player.hpp>

class ClientPlayer : public Player {
public:
    ClientPlayer(std::shared_ptr<World> world);

    void draw();
};