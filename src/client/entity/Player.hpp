#pragma once
#include <entity/SimplePlayer.hpp>
#include <memory>
#include <vector>
#include <raylib.h>
#include <Types.hpp>

class World;
class Chunk;
class Block;

struct BreakingBlockInfo {
    float totalDurability;
    float currentDurability;
};

class Player : public SimplePlayer {
private:
    Camera2D m_camera;

    bool m_sneakToggled = false;
    bool m_sneak = false;
    bool m_fly = false;
    bool m_inv = false;
    bool m_isBreakingBlock = false;
    bool m_isBreakingBlockPrev = false;
    bool m_onGroundPrev = false;
    
    PlayerID m_id;
    AnimationType m_prevAnimType;
    uint8_t m_prevAnimFrame;
    Direction m_prevDir;
    GameMode m_gamemode;
    BlockPosition m_breakingBlock;
    float m_breakingBlockDurability;
    float m_lastPunch;

    float m_lastAnimFrameTime;
    float m_lastDestroyedBlock;
    float m_lastPlacedBlock;
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
    bool isBlockInView(BlockPosition position);
    bool canDestroyBlock(BlockPosition target);
    bool canPlaceBlock(BlockPosition target);
    bool canAccessBlock(BlockPosition target);
    
    BlockPosition getTargetBlock();
    
    void placeBlock();
    void destroyBlock();

    Camera2D& getCamera() { return m_camera; }

    void triggerMove(Direction dir);
    void triggerJump();
    void triggerDuck(bool toggle);
    void toggleFly();

    GameMode getGameMode() { return m_gamemode; }
    void setGameMode(GameMode gamemode);

    bool isBreakingBlock() { return m_isBreakingBlock; }
    BreakingBlockInfo getBreakingBlockInfo();
};
