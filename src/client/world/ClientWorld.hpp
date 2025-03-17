#pragma once
#include <world/World.hpp>

class ClientWorld : public World {
public:
    ClientWorld(std::filesystem::path const& saveDir);

    void addChunk(Vec2i pos, std::shared_ptr<Chunk> chunk) override;
    void draw(std::shared_ptr<LocalPlayer> player);
};