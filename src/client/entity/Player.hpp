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

    PlayerID m_id = -1;

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

    Vector2 convertToCameraPos(Vector2 pos);
    Vector2 getTargetBlock(bool onlyExist = true);

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
