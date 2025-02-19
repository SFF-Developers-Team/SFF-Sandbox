#pragma once
#include <entity/SimplePlayer.hpp>
#include <memory>
#include <vector>
#include <raylib.h>
#include <Types.hpp>

class World;
class Chunk;
class Block;

class Player : public SimplePlayer {
private:
    Camera2D m_camera;
    std::vector<std::shared_ptr<Block>> m_inventory;
    int8_t m_selectedBlock;

    bool m_sneak = false;
    bool m_fly = false;
    bool m_inv = false;
    
    PlayerID m_id = 0;
    AnimationType m_prevAnimType;
    uint8_t m_prevAnimFrame;
    Direction m_prevDir;

    float m_lastAnimFrameTime = 0.f;
    float m_forward;

public:
    Player(std::shared_ptr<World> world);

    void onTick() override;
    void update();
    void onTickControls();
    void updateCamera();
    void updateAnimation();
    void updateControls();

    bool isChunkInView(std::shared_ptr<Chunk> chunk);
    bool isBlockInView(std::shared_ptr<Block> block);
    bool canDestroyBlock(Vec2i target, uint8_t layer);
    bool canPlaceBlock(Vec2i target, uint8_t layer);
    bool canAccessBlock(Vec2i target, uint8_t layer);
    
    Vec2i getTargetBlock();
    std::shared_ptr<Block> getSelectedBlock();
    int8_t getSelectedIndex() { return m_selectedBlock; }

    Camera2D& getCamera() { return m_camera; }
    std::vector<std::shared_ptr<Block>>& getInventory() { return m_inventory; }

    bool addToInventory(std::shared_ptr<Block> block);
};
