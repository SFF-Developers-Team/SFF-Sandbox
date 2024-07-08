#include <game.hpp>


void Game::init() {

    InitWindow(screenWidth, screenHeight, "FriendsTeam Sandbox");
    Grass = LoadTexture("resources/grass.png");
    Dirt = LoadTexture("resources/dirt.png");
    Stone = LoadTexture("resources/stone.png");
    SetTargetFPS(60);
    World World;
    World.Generate();
        while (!WindowShouldClose()) {
            this->render();
        }

        CloseWindow();
}
    
void Game::render() {
    World World;
    BeginDrawing();

        ClearBackground(SKYBLUE);    
        World.WorldDraw();

    EndDrawing();
}