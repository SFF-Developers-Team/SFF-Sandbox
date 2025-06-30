#pragma once
#include "AnimatedTexture.hpp"
#include "Entity.hpp"
#include <raylib-cpp.hpp>

#define PLAYER_BOX_WIDTH 0.9f
#define PLAYER_BOX_HEIGHT 1.35f

class PlayerClient : public Entity, private AnimatedTexture {
private:
public:
    PlayerClient(World& world);

    void OnTick() override;
    void Draw(raylib::Window& window);
};