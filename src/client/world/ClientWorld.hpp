#pragma once
#include <world/World.hpp>

class ClientWorld : public World {
public:
    ClientWorld(std::filesystem::path const& saveDir);

    void draw(std::shared_ptr<LocalPlayer> player);
};