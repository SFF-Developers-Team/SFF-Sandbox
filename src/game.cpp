#include <Game.hpp>
#include <Sprite.hpp>

void Game::init() {
    InitWindow(screenWidth, screenHeight, "FriendsTeam Sandbox");
    SetTargetFPS(60);

    Sprite::loadSprites();

    world.Generate();

    camera.target = {0, 0};
    camera.offset = {0, 0};
    camera.zoom = 1.0f;
    camera.rotation = 0;

    while (!WindowShouldClose()) {
        this->updateControls();
        this->render();
    }

    CloseWindow();
}
    
void Game::render() {
    BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(this->camera);
            world.WorldDraw();
        EndMode2D();
    EndDrawing();
}

void Game::updateControls() {
    if(IsKeyDown(KEY_W)) {
        this->camera.target.y -= cameraSpeed;
    }

    if(IsKeyDown(KEY_S)) {
        this->camera.target.y += cameraSpeed;
    }

    if(IsKeyDown(KEY_D)) {
        this->camera.target.x += cameraSpeed;
    }

    if(IsKeyDown(KEY_A)) {
        this->camera.target.x -= cameraSpeed;
    }
}