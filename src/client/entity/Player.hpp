#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <Entity.hpp>
#include <Block.hpp>
#include <SimplePlayer.hpp>

class World;
class Chunk;

class Player : public SimplePlayer {
private:
    Camera2D m_camera;
    Texture2D m_texture;
    Block::BlockType m_selectedBlock = Block::BlockType::STONE;

    bool m_canJump = false;
    bool m_sneak = false;
    bool m_fly = false;

    PlayerID m_id = 0;

    Rectf m_lastHitbox;
    uint8_t m_lastCurrentFrame;
    char m_lastDirection;

public:
    Player(World* world);
    ~Player();
    
    virtual void update() override; 
    void updateControls();
    void updateCamera();
    void updateAnimation();
    void updateMultiplayer();
    void processPhysics(bool hitWall, bool hitFloor, bool hitCeil) override;
    void draw();

    bool isChunkInView(Chunk* chunk);
    bool canDestroyBlock(Vec2i targetBlockPos, uint8_t layer);
    bool canPlaceBlock(Vec2i targetBlockPos, uint8_t layer);

    Vector2 convertToCameraPos(Vector2 pos);
    Vec2i getTargetBlock(bool onlyExist = true);

    auto getCamera() {
        return m_camera;
    }

    auto getSelectedBlock() {
        return m_selectedBlock;
    }

    void setID(PlayerID id) {
        m_id = id;
    }

    auto getID() {
        return m_id;
    }
};
