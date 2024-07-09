#pragma once
#include <vector>
#include <raylib.h>

enum Blocks {
    AIR,
    GRASS,
    DIRT,
    STONE
};

class World {
private:
    int worldArr[256][64];

public:
    void Generate();
    void WorldDraw();
    void BlockCheck();
    bool isBlockAccesible(int x, int y);

    std::vector<Vector2> hitboxes;
};  
