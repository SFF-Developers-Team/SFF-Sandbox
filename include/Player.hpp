#pragma once

#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <Entity.hpp>
#include <Block.hpp>
#include <Human.hpp>

class World;
class Chunk;

class Player : public Human {
private:
    Camera2D m_camera;
    Block::BlockType m_selectedBlock = Block::BlockType::STONE;

    bool m_canJump = false;
    bool m_sneak = false;
    bool m_fly = false;

public:
    Player(World* world);
    ~Player();
    
    void update(); 
    void updateControls();
    void updateCamera();
    void updateAnimation();
    void processPhysics(bool hitWall, bool hitFloor, bool hitCeil) override;
    void draw();

    bool isChunkInView(Chunk* chunk);

    Vector2 convertToCameraPos(Vector2 pos);
    Vector2 getTargetBlock(bool onlyExist = true);

    ByteVector& serialize();
    void deserialize(ByteVector& bytes);

    Camera2D getCamera() {
        return m_camera;
    }

    Block::BlockType getSelectedBlock() {
        return m_selectedBlock;
    }
};
