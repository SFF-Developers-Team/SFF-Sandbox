#pragma once
#include "AnimatedTexture.hpp"
#include "Player.hpp"
#include <raylib-cpp.hpp>

#define PLAYER_BOX_WIDTH 1.f
#define PLAYER_BOX_HEIGHT 1.375f

class PlayerClient : public Player, private AnimatedTexture {
public:
    PlayerClient(World& world, RVector2 position, raylib::Camera2D camera);
    
    BlockPosition GetTargetBlock() override;
    
    raylib::Camera2D& GetCamera() { return m_camera; }
    
    void OnTick() override;
    void Draw();
    void SetDigging(bool isDig) { m_isDig = isDig; }
private:
    bool m_isDig = 0;
    raylib::Camera2D m_camera;
};