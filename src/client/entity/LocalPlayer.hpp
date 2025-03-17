#pragma once
#include <entity/Player.hpp>
#include <memory>
#include <vector>
#include <raylib.h>
#include <Types.hpp>

class World;
class Chunk;
class Block;

class LocalPlayer : public Player {
private:
    Camera2D m_camera;

    bool m_sneakToggled = false;
    bool m_sneak = false;
    bool m_inGui = false;
    float m_forward;
    
public:
    LocalPlayer(std::shared_ptr<World> world);

    void onTick(World* world) override;
    void update();
    void onTickControls(World* world);
    void updateCamera();
    void updateAnimation();
    void updateControls();

    bool isChunkInView(Vec2i chunkPos);
    bool isBlockInView(BlockPosition position);
    
    BlockPosition getTargetBlock();
    
    void placeBlock(World* world);
    void destroyBlock(World* world);

    Camera2D& getCamera() { return m_camera; }

    void triggerMove(Direction dir);
    void triggerJump();
    void triggerDuck(bool toggle);
    
};
