#pragma once
#include "AnimatedTexture.hpp"
#include "Entity.hpp"

#define PLAYER_BOX_WIDTH 0.9f
#define PLAYER_BOX_HEIGHT 1.35f

class PlayerClient : public Entity, private AnimatedTexture {
public:
    PlayerClient(World& world);

    void OnTick() override;
    void Draw();
};