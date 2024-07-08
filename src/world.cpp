#include <world.hpp>
#include <iostream>

void World::Generate() {
    for (int x = 0;x<256;x++) {
        for(int y = 0;y<64;y++){
            if(y<1) {
                worldArr[x][y] = GRASS;       
            }
            if(y<21 && y>2) {
                worldArr[x][y] = DIRT;    
            }
            if(y<64 && y>21) {
                worldArr[x][y] = STONE;    
            }
    }
}
}
void World::WorldDraw() {

    for (int x = 0;x<256;x++) {
        for(int y = 0;y<64;y++) {
            if(y == 16 && y<38) {
                
                DrawTexturePro(Game::Grass, {0, 0, (float)Game::Grass.width, (float)Game::Grass.height}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE );
            }
            if(y<38 && y>16) {
                DrawTexturePro(Game::Dirt, {0, 0, (float)Game::Dirt.width, (float)Game::Dirt.height}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE );

            }
            if(y<64 && y>38) {
                DrawTexturePro(Game::Stone, {0, 0, (float)Game::Stone.width, (float)Game::Stone.height}, {(float)x * 32, (float)y * 32, 32, 32}, {0, 0}, 0, WHITE );
            }
        }
    }


}
// void World::