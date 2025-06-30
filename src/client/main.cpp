#include "PlayerClient.hpp"
#include "ResourceManager.hpp"
#include "Timer.hpp"
#include "Debug.hpp"
#include "SoundManager.hpp"
#include <World.hpp>
#include <algorithm>
#include <raylib-cpp.hpp>
#include <Utils.hpp>

int main() {
    raylib::Window window(1280, 720, "SFF Sandbox");
    raylib::AudioDevice audio;
    
    auto& rm = ResourceManager::Get();
    auto& sm = SoundManager::Get();

    rm.LoadTilemap("blocks.png", 16, 16);
    rm.LoadTilemap("gui.png", 8, 8);
    rm.LoadTexture("player.png");

    sm.LoadMusic("menu.mp3");
    Timer timer(60.f);

    World world("getoff");
    PlayerClient* player = new PlayerClient(world);

    world.AddEntity(player);
    // Set Y floor
    // We should compute Y, not set it manually
    // Also change restart button Y
    player->SetY(163);
    raylib::Camera2D camera(
        {window.GetWidth() / 2.f, window.GetHeight() / 2.f},
        {0.f, 0.f}
    );

    auto& tilemap = rm.GetTilemap("blocks.png");
    auto blockSelect = BLOCK_ID_GRASS;
    bool m_inventory = 0;

    HideCursor();
    sm.GetMusic("menu.mp3").Play();
    sm.GetMusic("menu.mp3").SetLooping(1);

    while (!window.ShouldClose()) { 
        sm.GetMusic("menu.mp3").Update();
        
        timer.AdvanceTime();

        for (int i = 0; i < timer.GetTicks(); i++) {
            world.OnTick();
        }

        if (float wheel = raylib::Mouse::GetWheelMove(); wheel != 0.f && raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.zoom += wheel;
        }

        // We cant use += to blockSelect so i made another var to wheel item swapping
        static int8_t count = 0;
        if (float wheel = raylib::Mouse::GetWheelMove(); wheel != 0.f && !raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
            count -= (wheel > 0 ? -1 : 1);
            if (count > BLOCK_ID_TORCH_1) count = BLOCK_ID_GRASS;
            if (count <= 0) count = BLOCK_ID_TORCH_1;
            blockSelect = (BlockID)count;
        }

        camera.zoom = std::clamp(camera.zoom, 5.f, 60.f);
        camera.SetTarget(player->GetPosition() + player->GetSize().Divide(2.f));
        raylib::Vector2 cursorPos = camera.GetScreenToWorld(raylib::Mouse::GetPosition());
        auto dbg = Debug::Get();
        dbg.AddString(std::format("X: {}\nY: {}", player->GetX(), player->GetY()));
        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) && cursorPos.Distance(player->GetPosition()) < 5) {
            world.BreakBlock(cursorPos.x, cursorPos.y);
        }
        // Here check if blocks around this pos exists and if pos doesnt equals pos of player
        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_RIGHT) && !player->CheckCollision({trunc(cursorPos.x), trunc(cursorPos.y), 1.f, 1.f}) 
        && cursorPos.Distance(player->GetPosition()) < 5 && world.GetBlocksAround({(int)cursorPos.x, (int)cursorPos.y, 1.f, 1.f}).size() != 0) {
            if (world.GetBlock(cursorPos.x, cursorPos.y) == BLOCK_ID_AIR) {
                world.SetBlock(cursorPos.x, cursorPos.y, blockSelect);
            } else if (raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
                world.SetBlock(cursorPos.x, cursorPos.y, blockSelect);
            }
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_E)) {
            m_inventory = !m_inventory;
        }
        window.BeginDrawing();
        window.ClearBackground(SKYBLUE);

        camera.BeginMode();

        world.Draw(camera, window.GetWidth(), window.GetHeight());
        player->Draw(window);
        if (world.GetBlocksAround({(int)cursorPos.x, (int)cursorPos.y, 1.f, 1.f}).size() != 0 
        && !player->CheckCollision({trunc(cursorPos.x), trunc(cursorPos.y), 1.f, 1.f}) ) {
            int8_t curBlock = world.GetBlock(cursorPos.x, cursorPos.y) == BLOCK_ID_AIR ? 1 : 2;
            bool isCtrlDown = raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL) && world.GetBlock(cursorPos.x, cursorPos.y) != 0 ? 1 : 0;
            rm.GetTilemap("gui.png").DrawTile(curBlock + isCtrlDown, {trunc(cursorPos.x), trunc(cursorPos.y), 1.f, 1.f});
        }
        camera.EndMode();

        dbg.Draw();

        if (m_inventory) {
            RRectangle background ((window.GetWidth() - 1000) / 2, (window.GetHeight() - 500) / 2, 1000, 500);
            int8_t countX = 0;
            int8_t countY = 2;
            auto textSize = raylib::MeasureText("Select block", 30);

            background.DrawRounded(0.05, 0, {0, 0, 0, 100});
            raylib::DrawText("Select block", (window.GetWidth() - textSize) / 2, background.GetY(), 30, WHITE);
            for (int i = 0; i < BLOCK_ID_TORCH_2; i++) {
                // Skipping animations or 2 part of block
                if (i != BLOCK_ID_AIR && i != BLOCK_ID_ADMINIUM && i != BLOCK_ID_BED_2) {
                    if (countX > 11) {
                        countY++;
                        countX = 0;
                    }
                    countX++;
                    RRectangle selectBlock = {120 + 80 * countX, (window.GetHeight() - 500) / 2 * countY, 32, 32};
                    if (selectBlock.CheckCollision(raylib::Mouse::GetPosition())) {
                        selectBlock.SetSize({32 * 1.5, 32 * 1.5});
                        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
                            blockSelect = static_cast<BlockID>(i);
                            m_inventory = !m_inventory;
                        }
                    }
                    rm.GetTilemap("blocks.png").DrawTile(i - 1, selectBlock);
                } 
            }
        }
        rm.GetTilemap("gui.png").DrawTile(0, {GetMousePosition().x, GetMousePosition().y, 16, 16});
        rm.GetTilemap("blocks.png").DrawTile(blockSelect - 1, {GetScreenWidth() - 48, 26, 32, 32});

        window.EndDrawing();
    }   

    window.Close();
}