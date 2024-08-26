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

    bool m_sneak = false;
    bool m_fly = false;

    PlayerID m_id = 0;
    
    float wheel;

    Rectf m_lastHitbox;
    uint8_t m_lastCurrentFrame;
    char m_lastDirection;

    bool m_inputDisabled = false;

    bool m_unlinkCamX = false;
    bool m_unlinkCamY = false;
public:
    Player(World* world);
    ~Player();
    
    void onTick() override; 
    void update();
    void updateControls();
    void updateCamera();
    void updateAnimation();

    void moveCameraRelative(float x, float y);

    bool isChunkInView(Chunk* chunk);
    bool isBlockInView(Block* block);
    bool canDestroyBlock(Vec2i targetBlockPos, uint8_t layer);
    bool canPlaceBlock(Vec2i targetBlockPos, uint8_t layer);

    Vector2 convertToCameraPos(Vector2 pos);
    Vec2i getTargetBlock(bool onlyExist = true);

    bool inputDisabled();
    void disableInput(bool flag);
    
    void unlinkCameraX(bool flag);
    void unlinkCameraY(bool flag);

    auto getCamera() {
        return m_camera;
    }

    void setCamera(Camera2D cam) {
        m_camera = cam;
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
