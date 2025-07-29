#include "Blocks.hpp"
#include "Chunk.hpp"
#include "PlayerClient.hpp"
#include "ResourceManager.hpp"
#include "Timer.hpp"
#include "Types.hpp"
#include "worldgen/Default.hpp"
#include "Items.hpp"
#include "ItemFactory.hpp"
#include "Food.hpp"
#include "BlockItem.hpp"
#include "FoodDatabase.hpp"
#include <Rectangle.hpp>
#include <RenderTexture.hpp>
#include <Vector2.hpp>
#include <World.hpp>
#include <memory>
#include <raylib-cpp.hpp>
#include <Utils.hpp>
#include <raylib.h>

#define HOTBAR_CELL_COUNT 9

int main() {
    raylib::Window window(1280, 720, "SFF Sandbox");
    raylib::AudioDevice audio;

    auto& rm = ResourceManager::Get();
    auto& itmf = ItemFactory::Get();
    auto& blocksTilemap = rm.LoadTilemap("blocks.png", 16, 16);
    auto& guiTilemap = rm.LoadTilemap("gui.png", 8, 8);
    auto& indicatorsTileMap = rm.LoadTilemap("indicators.png", 6, 1);
    auto& itemsTilemap = rm.LoadTilemap("items.png", 16, 16);
    auto& menuMusic = rm.LoadMusic("menu.mp3");
    auto& blurShader = rm.LoadShader("vertexShader.glsl", "blurFragShader.glsl");
    rm.LoadTexture("player.png");

    // float blurRadius = 10;
    // blurShader.SetValue(blurShader.GetLocation("blurRadius"), &blurRadius, SHADER_UNIFORM_FLOAT);

    std::srand(std::time(0));

    // Setup timer
    Timer timer(60.f);

    // Create world
    World world("getoff", std::make_unique<Default>(std::rand()));

    // Calculate spawn position
    RVector2 spawn;

    for (int y = 0; y < 1000; y++) {
        if (world.GetBlock(0, y, CHUNK_FOREGROUND_LAYER) != BLOCK_ID_AIR) {
            spawn.y = y - 2;
            break;
        }
    }

    // Create player
    PlayerClient* player = new PlayerClient(world, spawn, {{window.GetWidth() / 2.f, window.GetHeight() / 2.f}, {0.f, 0.f}, 0.f, 50.f});

    world.AddEntity(player);

    // Setup camera
    raylib::Camera2D& camera = player->GetCamera();

    int selectedBlock = 0;
    bool inventory = false;
    std::optional<raylib::Vector2> m_breakingBlockPosition = {};
    float m_breakingBlockProgress = 0;
    int lastPunch = 0;

    itmf.RegisterItem(ID_FOOD_APPLE, std::move(std::make_unique<Apple>(0, 40)));
    itmf.RegisterItem(ID_BLOCKITEM_GRASS, std::move(std::make_unique<BlockItem>(BLOCK_ID_GRASS)));
    itmf.RegisterItem(ID_BLOCKITEM_DIRT, std::move(std::make_unique<BlockItem>(BLOCK_ID_DIRT)));

    HideCursor();

    menuMusic.Play().SetVolume(0.5f).SetLooping(true);

    std::array<InventoryItem, 36> inventoryItems = {};

    inventoryItems.at(1) = {itmf.Create<BlockItem>(ID_BLOCKITEM_GRASS), 5};

    InventoryItem tempItem = {};
    uint8_t currentItem = 1;
    uint8_t cellSlot = 0;

    player->SetHealth(1);

    raylib::RenderTexture2D light(window.GetWidth(), window.GetHeight());
    auto lightTex = light.GetTexture();

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
            currentItem += (wheel < 0.f) ? -1 : 1 + cellSlot;
            if (currentItem >= cellSlot + 9) currentItem = cellSlot;
            if (currentItem < cellSlot) currentItem = cellSlot + 8;
        }

        camera.zoom = std::clamp(camera.zoom, 5.f, 60.f);
        camera.SetTarget(player->GetPosition() + player->GetSize() / 2.f);

        int const selectedLayer = !raylib::Keyboard::IsKeyDown(KEY_LEFT_ALT);

        // Screen float cursor
        raylib::Vector2 const cursor = raylib::Mouse::GetPosition();

        // World float cursor
        raylib::Vector2 cursorWorld = camera.GetScreenToWorld(cursor);

        // World int cursor
        Vector2i cursorWorldI(static_cast<int>(std::floor(cursorWorld.x)), static_cast<int>(std::floor(cursorWorld.y)));

        auto blockinCursor = world.GetBlock(cursorWorldI.x, cursorWorldI.y, selectedLayer);
        auto distanceBetweenCursor = player->GetPosition().Distance(cursorWorld);
        auto& item = inventoryItems.at(currentItem);

        // Here check if blocks around this pos exists and if pos doesnt equals pos of player
        bool const canAccessBlock = distanceBetweenCursor <= 5.f;

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
            if (blockinCursor != BLOCK_ID_AIR && !raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL)) {
                return false;
            }

            // Check if there is at least one block near
            bool const hasAdjacentBlock = {world.GetBlock(cursorWorldI.x + 1, cursorWorldI.y, selectedLayer) != BLOCK_ID_AIR ||
                                           world.GetBlock(cursorWorldI.x - 1, cursorWorldI.y, selectedLayer) != BLOCK_ID_AIR ||
                                           world.GetBlock(cursorWorldI.x, cursorWorldI.y + 1, selectedLayer) != BLOCK_ID_AIR ||
                                           world.GetBlock(cursorWorldI.x, cursorWorldI.y - 1, selectedLayer) != BLOCK_ID_AIR ||
                                           world.GetBlock(cursorWorldI.x, cursorWorldI.y, !selectedLayer) != BLOCK_ID_AIR};

            return hasAdjacentBlock;
        }();

        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) && !inventory && blockinCursor != BLOCK_ID_AIR) {
            if (distanceBetweenCursor < 5.f && player->GetHealth() > 0) {
                if (cursorWorldI != m_breakingBlockPosition) {
                    m_breakingBlockPosition = cursorWorldI;
                    m_breakingBlockProgress = 0.f;
                }

                // This shit should be removed when we move this code into player class
                player->SetDigging(true);

                auto speed = 0.1f;
                // Doing this because this statement in main loop
                if (world.GetTicks() >= lastPunch + 1) {
                    m_breakingBlockProgress += speed;
                    lastPunch = world.GetTicks();
                }

                if (m_breakingBlockProgress >= 1.f) {
                    world.SetBlock(cursorWorldI.x, cursorWorld.y, selectedLayer, BLOCK_ID_AIR);
                    for (int i = 0; i < inventoryItems.size(); i++) {
                        auto& cell = inventoryItems.at(i);
                        auto itemCheck = std::dynamic_pointer_cast<BlockItem>(cell.item);

                        if (itemCheck != nullptr && itemCheck->GetID() == blockinCursor) {
                            cell.count++;
                            break;
                        }

                        if (itemCheck == nullptr && cell.count <= 0) {
                            cell = {itmf.Create<BlockItem>(blocksDatabase.at(blockinCursor).fabricID), 1};
                            break;
                        }
                    }
                    m_breakingBlockProgress = 0.f;
                    player->SetDigging(false);
                }
            }
        } else {
            m_breakingBlockProgress = 0.f;
            m_breakingBlockPosition = std::nullopt;
        }

        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_RIGHT) && !inventory && player->GetHealth() > 0 && item.count > 0) {
            auto block = std::dynamic_pointer_cast<BlockItem>(item.item);
            
            if (block != nullptr) {
                // I dont know where canPlaceBlock bool should be in code, so it here
                if (canPlaceBlock && block->OnUse(player)) {
                    if (blockinCursor == BLOCK_ID_AIR) {
                        item.count--;
                    }

                    if (item.count <= 0) {
                        item.item = nullptr;
                    }
                }
            } else {
                auto usable = std::dynamic_pointer_cast<UsableItem>(item.item);

                if (usable->OnUse(player)) {
                    item.count--;
                    if (item.count <= 0) {
                        item.item = nullptr;
                    }
                }
            }
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_E) && player->GetHealth() > 0) {
            inventory ^= 1;
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_R) && player->GetHealth() <= 0) {
            player->SetHealth(20);
            player->SetPosition(spawn);
        }

        if (raylib::Keyboard::IsKeyPressed(KEY_TAB) && player->GetHealth() > 0) {
            cellSlot += 9;
            currentItem += 9;

            if (cellSlot >= 36) {
                cellSlot = 0;
                currentItem -= 36;
            }
        }

        raylib::Color dayColor = {102, 191, 255, 255};
        raylib::Color nightColor = {10, 10, 30, 255};
        raylib::Color skyColor = nightColor.Lerp(dayColor, world.GetDaylightFactor());

        for (int i = 0; i < 9; i++) {
            if (IsKeyDown(KEY_ONE + i)) {
                currentItem = i + cellSlot;
            }
        }

        world.PrepareLightmap(camera, light, window.GetWidth(), window.GetHeight());

        window.BeginDrawing();
        window.ClearBackground(skyColor);

        camera.BeginMode();

        world.Draw(camera, window.GetWidth(), window.GetHeight());

        if (player->GetHealth() > 0) {
            player->Draw();
        }

        // Draw tile selector
        if (canAccessBlock && !inventory && player->GetHealth() > 0) {
            bool const isAir = (world.GetBlock(cursorWorldI.x, cursorWorldI.y, selectedLayer) == BLOCK_ID_AIR);
            bool const isCtrlDown = raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL);
            bool const canReplaceBlock = (isCtrlDown && !isAir);

            int const tileSpriteId = (isAir ? 1 : 2) + canReplaceBlock;

            guiTilemap.DrawTile(tileSpriteId, RRectangle {cursorWorldI, {1.f, 1.f}});
        }

        if (m_breakingBlockProgress > 0) {
            guiTilemap.DrawTile(m_breakingBlockProgress * 6.f + 8, {static_cast<float>(cursorWorldI.x), static_cast<float>(cursorWorldI.y), 1.f, 1.f});
        }
        camera.EndMode();

        // Draw Lightmap
        blurShader.BeginMode();
        lightTex.Draw(RRectangle {0.f, 0.f, (float)lightTex.width, (float)-lightTex.height}, RRectangle {0.f, 0.f, (float)window.GetWidth(), (float)window.GetHeight()});
        blurShader.EndMode();

        // Draw inventory
        if (inventory) {
            RRectangle background = {(window.GetWidth() - 800.f) / 2.f, (window.GetHeight() - 64 * 4) / 2.f, 800.f, 64 * 4};

            background.Draw(DARKGRAY);
            background.DrawLines(GRAY, 5.f);

            float x = 0;
            float y = 0;

            for (auto i = 0; i < inventoryItems.size(); i++) {
                if (x >= 9) {
                    x = 0;
                    y++;
                }
                x++;

                RRectangle cell = {background.x + 64 * (x - 1), background.y + 64 * y, 64, 64};
                cell.DrawLines(GRAY, 5.f);
                auto& invItem = inventoryItems.at(i);
                if (cell.CheckCollision(cursor)) {
                    cell.DrawLines(WHITE, 2.f);

                    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
                        if (tempItem.item != invItem.item) {
                            std::swap(tempItem, invItem);
                        } else {
                            invItem.count += tempItem.count;
                            tempItem.item = nullptr;
                            tempItem.count = 0;
                        }
                    }

                    if (raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_RIGHT)) {
                        if (tempItem.item == nullptr) {
                            tempItem = {invItem.item, 1};
                            invItem.count--;
                            if (invItem.count <= 0) {
                                invItem.item = nullptr;
                            }
                        } else if (tempItem.item == invItem.item) {
                            tempItem.count++;
                            invItem.count--;
                            if (invItem.count <= 0) {
                                invItem.item = nullptr;
                            }
                        }
                    }
                }

                if (inventoryItems.at(i).count > 0) {
                    auto textSize = raylib::MeasureText(std::to_string(inventoryItems.at(i).count), 25);
                    auto& itemDraw = inventoryItems.at(i).item;
                    auto itemCheck = std::dynamic_pointer_cast<BlockItem>(inventoryItems.at(i).item);
                    // Non BlockItem
                    if (itemCheck == nullptr) {
                        raylib::Rectangle dest = {static_cast<float>(cell.x + 8), static_cast<float>(cell.y + 8), 48, 48};
                        itemsTilemap.DrawTile(itemDraw->GetSpriteIndex(), dest);
                    } else {
                        raylib::Rectangle dest = {static_cast<float>(cell.x + 16), static_cast<float>(cell.y + 16), 32, 32};
                        blocksTilemap.DrawTile(itemCheck->GetID() - 1, dest);
                    }
                    if (inventoryItems.at(i).count > 1) {
                        raylib::DrawText(std::format("{}", inventoryItems.at(i).count), cell.x + (64 - textSize - 10), cell.y + (64 - 30), 25, WHITE);
                    }
                }
            }

            raylib::DrawText("Crafting\nsystem\nsoon :)", background.x + 64 * 9 + 10, background.y + 25, 40, WHITE);

            if (tempItem.item != nullptr) {
                auto textSize = raylib::MeasureText(std::to_string(tempItem.count), 25);
                auto itemCheck = std::dynamic_pointer_cast<BlockItem>(tempItem.item);

                // Non BlockItem
                if (itemCheck == nullptr) {
                    itemsTilemap.DrawTile(tempItem.item->GetSpriteIndex(), {cursor.x - 24, cursor.y - 24, 48.f, 48.f});
                } else {
                    blocksTilemap.DrawTile(tempItem.item->GetID() - 1, {cursor.x - 16, cursor.y - 16, 32.f, 32.f});
                }

                if (tempItem.count > 1) {
                    raylib::DrawText(std::format("{}", tempItem.count), cursor.x + textSize, cursor.y, 25, WHITE);
                }
            }
        }

        auto health = player->GetHealth();
        auto add = (world.GetTicks() < player->GetLastTimeHurt() + 10) * 3;

        for (auto i = 0; i < 10; i++) {
            if (health / 2 >= i + 1.f) {
                indicatorsTileMap.DrawTile(add, {static_cast<float>(16 * i), 0, 16, 14});
                continue;
            }
            if (health / 2 >= i + 0.5f) {
                indicatorsTileMap.DrawTile(add + 1, {static_cast<float>(16 * i), 0, 16, 14});
                continue;
            }
            indicatorsTileMap.DrawTile(add + 2, {static_cast<float>(16 * i), 0, 16, 14});
        }

        if (health <= 0) {
            RRectangle dieScr = {0, 0, static_cast<float>(window.GetWidth()), static_cast<float>(window.GetHeight())};
            std::string deathText = "TODO: Make good death screen, when we do good gui framework :P\nTo respawn, press R key";

            dieScr.Draw({255, 0, 0, 140});
            raylib::DrawText(deathText, (window.GetWidth() - raylib::MeasureText(deathText, 25)) / 2, (window.GetHeight() - 25) / 2, 25, WHITE);
        }

        RRectangle itemsBG(window.GetWidth() / 3.5f, 0, 64 * HOTBAR_CELL_COUNT, 64.f);
        itemsBG.Draw(DARKGRAY);

        // 9 containers with items
        for (auto i = cellSlot; i < cellSlot + 9; i++) {
            RRectangle selectedRec(window.GetWidth() / 3.5f + (i - cellSlot) * 64, 0, 64.f, 64.f);
            selectedRec.DrawLines(GRAY, 5);

            auto invItem = inventoryItems.at(i);
            auto textSize = raylib::MeasureText(std::to_string(invItem.count), 25);

            if (invItem.count > 0) {
                auto& itemDraw = inventoryItems.at(i).item;
                auto itemCheck = std::dynamic_pointer_cast<BlockItem>(inventoryItems.at(i).item);
                // Non BlockItem
                if (itemCheck == nullptr) {
                    raylib::Rectangle dest = {static_cast<float>(selectedRec.x + 8), static_cast<float>(selectedRec.y + 8), 48, 48};
                    itemsTilemap.DrawTile(itemDraw->GetSpriteIndex(), dest);
                } else {
                    raylib::Rectangle dest = {static_cast<float>(selectedRec.x + 16), static_cast<float>(selectedRec.y + 16), 32, 32};
                    blocksTilemap.DrawTile(itemCheck->GetID() - 1, dest);
                }
                if (inventoryItems.at(i).count > 1) {
                    raylib::DrawText(std::format("{}", inventoryItems.at(i).count), selectedRec.x + (64 - textSize - 10), selectedRec.y + (64 - 30), 25, WHITE);
                }
                // }
                if (invItem.count > 1) {
                    raylib::DrawText(std::format("{}", invItem.count), selectedRec.x + (64 - textSize - 10), 64 - 30, 25, WHITE);
                }
            }
        }

        RRectangle selectedRec(window.GetWidth() / 3.5 + (currentItem - cellSlot) * 64, 0, 64.f, 64.f);
        selectedRec.DrawLines(WHITE, 2);

        for (int i = 0; i < 36; i += 9) {
            RRectangle hotbarNonSelected = {itemsBG.x - 29, static_cast<float>((i * 1.65) + 5), 10.f, 10.f};
            hotbarNonSelected.Draw(GRAY);
        }

        RRectangle hotbarPos = {itemsBG.x - 29, static_cast<float>((cellSlot * 1.65) + 5), 10.f, 10.f};
        hotbarPos.Draw(WHITE);

        guiTilemap.DrawTile(0, {cursor.Subtract({8.f, 8.f}), {16.f, 16.f}});

        raylib::DrawText(std::format("Pos: {} {}\nChunk: {} {}\nChunk light updates: {}\nFPS: {}", player->GetPosition().x, player->GetPosition().y, 
        (int)(player->GetPosition().x / CHUNK_WIDTH), (int)(player->GetPosition().y / CHUNK_HEIGHT), world.chunkLightUpdates, window.GetFPS()), 20, 20, 20, DARKGREEN);

        window.EndDrawing();
    }

    window.Close();

    return 0;
}