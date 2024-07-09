#include <Game.hpp>
#include <Sprite.hpp>

void Game::init() {
    InitWindow(screenWidth, screenHeight, "FriendsTeam Sandbox");
    SetTargetFPS(60);

    Sprite::loadSprites();

    world.Generate();

    while (!WindowShouldClose()) {
        this->render();
    }

    CloseWindow();
}
    
void Game::render() {
    BeginDrawing();
        ClearBackground(SKYBLUE);    
        
        world.WorldDraw();
    EndDrawing();
}