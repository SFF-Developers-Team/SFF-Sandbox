#include "Player.hpp"
#include "ResourceManager.hpp"
#include "Tilemap.hpp"
#include "Timer.hpp"
#include <World.hpp>
#include <algorithm>
#include <raylib-cpp.hpp>

int main() {
    raylib::Window window(1280, 720, "SFF Sandbox");

    auto& rm = ResourceManager::Get();

    rm.LoadTilemap("blocks.png", 16, 16);
    rm.LoadTexture("player.png");

    Timer timer(60.f);

    World world("getoff");
    Player* player = new Player(world);

    world.AddEntity(player);

    raylib::Camera2D camera(
        {window.GetWidth() / 2.f, window.GetHeight() / 2.f},
        {0.f, 0.f}
    );

    auto& tilemap = rm.GetTilemap("blocks.png");

    while (!window.ShouldClose()) { 
        timer.AdvanceTime();

        for (int i = 0; i < timer.GetTicks(); i++) {
            world.Update();
        }

        if (float wheel = raylib::Mouse::GetWheelMove(); wheel != 0.f) {
            camera.zoom += wheel;
        }

        camera.zoom = std::clamp(camera.zoom, 5.f, 30.f);
        camera.SetTarget(player->GetPosition() + player->GetSize().Divide(2.f));
        
        window.BeginDrawing();
        window.ClearBackground(SKYBLUE);

        camera.BeginMode();

        world.Draw(camera, window.GetWidth(), window.GetHeight());
        player->Draw();

        camera.EndMode();

        window.EndDrawing();
    }

    window.Close();
}