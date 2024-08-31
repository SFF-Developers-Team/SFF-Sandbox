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
    Block::Type m_selectedBlock = Block::Type::STONE;

    bool m_sneak = false;
    bool m_fly = false;

    PlayerID m_id = 0;
    uint8_t m_prevAnimFrame;
    Entity::Direction m_prevDir;

    float m_lastAnimFrameTime = 0.f;

public:
    Player(World* world);
    ~Player();
    
    void onTick() override; 
    void update();
    void updateControls();
    void updateCamera();
    void updateAnimation();

    void moveCameraRelative(float x, float y);

    bool isChunkInView(std::shared_ptr<Chunk> chunk);
    bool isBlockInView(std::shared_ptr<Block> block);
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
