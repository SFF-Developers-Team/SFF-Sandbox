#pragma once

#include <memory>
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
    std::vector<std::shared_ptr<Block>> m_inventory;
    int8_t m_selectedBlock = 0;

    bool m_sneak = false;
    bool m_fly = false;

    PlayerID m_id = 0;
    uint8_t m_prevAnimFrame;
    Entity::Direction m_prevDir;

    float m_lastAnimFrameTime = 0.f;

    bool m_inputDisabled = false;
    bool m_unlinkCamX = false;
    bool m_unlinkCamY = false;
public:
    Player(std::shared_ptr<World> world);
    ~Player();
    
    void onTick() override; 
    void update();
    void updateControls();
    void updateCamera();
    void updateAnimation();

    bool isChunkInView(std::shared_ptr<Chunk> chunk);
    bool isBlockInView(std::shared_ptr<Block> block);
    bool canDestroyBlock(Vec2i target, uint8_t layer);
    bool canPlaceBlock(Vec2i target, uint8_t layer);
    bool canAccessBlock(Vec2i target, uint8_t layer);
    Vec2i getTargetBlock();

    bool inputDisabled();
    void disableInput(bool flag);
    
    void unlinkCameraX(bool flag);
    void unlinkCameraY(bool flag);

    void setSelectedBlock(uint16_t block) { m_selectedBlock = block; }
    void setCamera(Camera2D cam) { m_camera = cam; }
    void setID(PlayerID id) { m_id = id; }

    std::shared_ptr<Block> getSelectedBlock();
    
    auto getInventory() { return m_inventory; }
    auto getCamera() { return m_camera; }
    auto getID() { return m_id; }
};
