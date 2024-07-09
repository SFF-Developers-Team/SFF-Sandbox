#include <World.hpp>
#include <iostream>
#include <Sprite.hpp>

void World::Generate() {
    for (int x = 0; x < 256; x++) {
        for(int y = 0; y < 64; y++){
            if(y < 1) {
                worldArr[x][y] = GRASS;       
            }

            if(y < 21 && y > 2) {
                worldArr[x][y] = DIRT;    
            }

            if(y < 64 && y > 21) {
                worldArr[x][y] = STONE;    
            }
        }
    }
}

void World::WorldDraw() {
    for (int x = 0; x < 256; x++) {
        for(int y = 0; y < 64; y++) {
            if(y == 16 && y < 38) {
                DrawTexturePro(Sprite::grass->getTexture(), {0, 0, (float)Sprite::grass->getWidth(), (float)Sprite::grass->getHeight()}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE );
            }

            if(y < 38 && y > 16) {
                DrawTexturePro(Sprite::dirt->getTexture(), {0, 0, (float)Sprite::dirt->getWidth(), (float)Sprite::dirt->getHeight()}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE );
            }

            if(y < 64 && y > 38) {
                DrawTexturePro(Sprite::stone->getTexture(), {0, 0, (float)Sprite::stone->getWidth(), (float)Sprite::stone->getHeight()}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE );
            }
        }
    }
}

bool World::isBlockAccesible(int x, int y) {
    return worldArr[x][y + 1] == AIR || worldArr[x][y - 1] == AIR || worldArr[x + 1][y] == AIR || worldArr[x - 1][y] == AIR;
}

void World::BlockCheck() {
    for (int x = 0; x < 256; x++) {
        for(int y = 0; y < 64; y++) {
            if(isBlockAccesible(x, y) && worldArr[x][y] != AIR) {
                hitboxes.push_back(Vector2 {(float)x, (float)y});
            }
        }
    }
}