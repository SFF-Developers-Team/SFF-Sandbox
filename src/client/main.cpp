#include "PlayerClient.hpp"
#include "ResourceManager.hpp"
#include "Timer.hpp"
#include "Types.hpp"
#include <Vector2.hpp>
#include <World.hpp>
#include <raylib-cpp.hpp>
#include <Utils.hpp>
#include <raylib.h>

int main() {
    raylib::Window window(1280, 720, "SFF Sandbox");
    raylib::AudioDevice audio;

    auto& rm = ResourceManager::Get();
    auto& blocksTilemap = rm.LoadTilemap("blocks.png", 16, 16);
    auto& guiTilemap = rm.LoadTilemap("gui.png", 8, 8);
    auto& menuMusic = rm.LoadMusic("menu.mp3");
    rm.LoadTexture("player.png");

    // Setup timer
    Timer timer(60.f);

    // Create world
    World world("getoff");
    
    // Create player
    PlayerClient* player = new PlayerClient(world);

    world.AddEntity(player);

    // Calculate spawn position
    RVector2 spawn;

    for (int y = 0; y < 1000; y++) {
        if (world.GetBlock(0, y) != BLOCK_ID_AIR) {
            spawn.y = y - 2;
            break;
        }
    }

    player->SetPosition(spawn);

    // Setup camera
    raylib::Camera2D camera({window.GetWidth() / 2.f, window.GetHeight() / 2.f}, {0.f, 0.f}, 0.f, 50.f);

    std::array<BlockID, 24> static const blockList = {
        BLOCK_ID_GRASS,     BLOCK_ID_DIRT,      BLOCK_ID_STONE,     BLOCK_ID_COBBLESTONE, BLOCK_ID_PLANKS,    BLOCK_ID_WOOL,
        BLOCK_ID_BRICKS,    BLOCK_ID_WOOD_1,    BLOCK_ID_WOOD_2,    BLOCK_ID_LEAVES,      BLOCK_ID_BOOKSHELF, BLOCK_ID_FLOWER_POT,
        BLOCK_ID_FURNACE_1, BLOCK_ID_WORKBENCH, BLOCK_ID_CHEST,     BLOCK_ID_BED_1,     BLOCK_ID_DEAD_ROSE,
        BLOCK_ID_ROSE,      BLOCK_ID_IRON_ORE,  BLOCK_ID_COAL_ORE,  BLOCK_ID_DIAMOND_ORE, BLOCK_ID_GOLD_ORE,  BLOCK_ID_TORCH_1
    };

    int selectedBlock = 0;
    bool inventory = false;

    HideCursor();
    menuMusic.Play().SetLooping(true);

    while (!window.ShouldClose()) {
        rm.UpdateMusic();
        timer.AdvanceTime();

        for (int i = 0; i < timer.GetTicks(); i++) {
            world.OnTick();
        }

        float wheel = raylib::Mouse::GetWheelMove();

        if (wheel != 0.f && raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.zoom += wheel;
        }

        if (wheel != 0.f && !raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
            selectedBlock += (wheel < 0.f) ? -1 : 1;

            if (selectedBlock > blockList.size() - 1) selectedBlock = 0;
            if (selectedBlock < 0) selectedBlock = blockList.size() - 1;
        }

        camera.zoom = std::clamp(camera.zoom, 5.f, 60.f);
        camera.SetTarget(player->GetPosition() + player->GetSize() / 2.f);


        // Screen float cursor
        raylib::Vector2 cursor = raylib::Mouse::GetPosition();

        // World float cursor
        raylib::Vector2 cursorWorld = camera.GetScreenToWorld(cursor);

        if (cursorWorld.x < 0.f) cursorWorld.x -= 1.f;
        if (cursorWorld.y < 0.f) cursorWorld.y -= 1.f;

        // World int cursor
        Vector2i cursorWorldI(cursorWorld);

        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) && player->GetPosition().Distance(cursorWorld) < 5.f) {
            world.BreakBlock(cursorWorldI.x, cursorWorldI.y);
        }

        // Here check if blocks around this pos exists and if pos doesnt equals pos of player

        bool const canAccessBlock = player->GetPosition().Distance(cursorWorld) <= 5.f;

        bool const canPlaceBlock = [&]() {
            // Check if player intesects with block
            if (player->CheckCollision(RRectangle {cursorWorldI, {1.f, 1.f}})) {
                return false;
            }

            // Check if player close enough to block
            if (!canAccessBlock) {
                return false;
            }

            // Check if block in cursor position is air (or change block, if ctrl pressed)
            if (world.GetBlock(cursorWorldI.x, cursorWorldI.y) != BLOCK_ID_AIR && !raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
                return false;
            }

            // Check if there is at least one block near
            bool const hasAdjacentBlock = {
                world.GetBlock(cursorWorldI.x + 1, cursorWorldI.y) != BLOCK_ID_AIR || 
                world.GetBlock(cursorWorldI.x - 1, cursorWorldI.y) != BLOCK_ID_AIR ||
                world.GetBlock(cursorWorldI.x, cursorWorldI.y + 1) != BLOCK_ID_AIR || 
                world.GetBlock(cursorWorldI.x, cursorWorldI.y - 1) != BLOCK_ID_AIR
            };

            return hasAdjacentBlock;
        }();

        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_RIGHT) && canPlaceBlock) {
            world.SetBlock(cursorWorldI.x, cursorWorldI.y, blockList[selectedBlock]);
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_E)) {
            inventory ^= 1;
        }

        window.BeginDrawing();
        window.ClearBackground(SKYBLUE);

        camera.BeginMode();

        world.Draw(camera, window.GetWidth(), window.GetHeight());
        player->Draw();

        if (canAccessBlock && !inventory) {
            bool const isAir = (world.GetBlock(cursorWorldI.x, cursorWorldI.y) == BLOCK_ID_AIR);
            bool const isCtrlDown = raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL);
            bool const canReplaceBlock = (isCtrlDown && !isAir);

            int const tileSpriteId = (isAir ? 1 : 2) + canReplaceBlock;

            guiTilemap.DrawTile(tileSpriteId, RRectangle {cursorWorldI, {1.f, 1.f}});
        }

        camera.EndMode();

        if (inventory) {
            RRectangle background = {(window.GetWidth() - 1000.f) / 2.f, (window.GetHeight() - 500.f) / 2.f, 1000.f, 500.f};

            background.DrawRounded(0.05, 0, {0, 0, 0, 100});

            raylib::Text title("Select block", 30.f);
            title.SetSpacing(2.f);
            title.Draw((window.GetWidth() - title.Measure()) / 2.f, background.y);

            RVector2 start = {
                (background.width - 880.f) / 2.f,
                (background.height - 80.f * (int)(blockList.size() / 11)) / 2.f
            };

            for (int i = 0; i < blockList.size(); i++) {
                RRectangle blockIcon = {
                    background.x + start.x + 80.f * (int)(i % 11), 
                    background.y + start.y + 80.f * (int)(i / 11), 
                    32.f, 32.f
                };

                if (blockIcon.CheckCollision(raylib::Mouse::GetPosition())) {
                    blockIcon.SetPosition({blockIcon.x - 8.f, blockIcon.y - 8.f});
                    blockIcon.SetSize({48.f, 48.f});
                        
                    if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
                        selectedBlock = i;
                        inventory = false;
                    }
                }
                
                blocksTilemap.DrawTile(blockList[i] - 1, blockIcon);
            }
        }

        guiTilemap.DrawTile(0, {cursor, {16.f, 16.f}});
        blocksTilemap.DrawTile(blockList[selectedBlock] - 1, {window.GetWidth() - 48.f, 16.f, 32.f, 32.f});

        window.EndDrawing();
    }

    window.Close();

    return 0;
}