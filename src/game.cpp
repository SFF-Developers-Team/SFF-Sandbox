#include <Game.hpp>
#include <Sprite.hpp>
#include <player.hpp>
void Game::init() {
    InitWindow(screenWidth, screenHeight, "FriendsTeam Sandbox");
    SetTargetFPS(60);

    Sprite::loadSprites();

    world.Generate();
    this->player = new Player();
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