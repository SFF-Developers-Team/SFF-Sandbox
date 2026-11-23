#pragma once
#include <Rectangle.hpp>
#include <world/World.hpp>

class ClientWorld : public World {
public:
    ClientWorld(std::filesystem::path const& saveDir);

    void draw(std::shared_ptr<LocalPlayer> player);
    void drawTile(raylib::Rectangle dest, TilePosition position);
};