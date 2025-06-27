#pragma once
#include "Entity.hpp"

class Player : public Entity {
public:
    Player(World& world);

    void Update() override;

#ifdef SANDBOX_CLIENT
    void Draw();
#endif
};