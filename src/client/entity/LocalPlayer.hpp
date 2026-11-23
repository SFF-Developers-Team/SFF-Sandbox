#pragma once
#include <Rectangle.hpp>
#include <raylib-cpp.hpp>
#include <entity/ClientPlayer.hpp>
#include <Types.hpp>
#include <memory>

class World;

class LocalPlayer : public ClientPlayer {
private:
    raylib::Camera2D m_camera;

    bool m_sneakToggled = false;
    bool m_sneak = false;
    bool m_inGui = false;
    float m_forward;
    
public:
    LocalPlayer(World& world);

    void onTick(World& world) override;
    void update();
    void onTickControls(World& world);
    void updateCamera();
    void updateAnimation();
    void updateControls();

    bool isRectInView(raylib::Rectangle rect);
    
    TilePosition getTargetTile();
    
    void placeTile(World& world);
    void destroyTile(World& world);

    raylib::Camera2D& getCamera() { return m_camera; }

    void triggerMove(Direction dir);
    void triggerJump();
    void triggerDuck(bool toggle);
};
