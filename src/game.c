#include "game.h"
#include "defines.h"
#include "textures.h"
#include "raymath.h"
#include "tilemap.h"
#include <stddef.h> // for NULL
#include "skin.h"
#include "screens.h"
#include "timer.h"

Tilemap* blocksTilemap = NULL;

LocalPlayer localPlayers[MAX_LOCAL_PLAYERS] = { 0 };
int localPlayersCount = 0;

bool isPaused = false;

void Game_UpdateWindow(void) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    Vector2 screenSize;

    switch (localPlayersCount) {
        case 1: 
            screenSize = (Vector2){screenW, screenH}; 
            localPlayers[0].camera.offset = (Vector2){screenSize.x / 2.0f, screenSize.y / 2.0f};
            break;
        case 2: 
            screenSize = (Vector2){screenW / 2.0f, screenH}; 

            localPlayers[0].camera.offset = (Vector2){screenSize.x / 2.0f, screenSize.y / 2.0f};
            localPlayers[1].camera.offset = (Vector2){screenSize.x + screenSize.x / 2.0f, screenSize.y / 2.0f};
            break;
    }

    for (int i = 0; i < localPlayersCount; i++) 
        localPlayers[i].camera.zoom = 50.f * (screenSize.x < screenSize.y ? screenSize.y / SCREEN_HEIGHT : screenSize.x / SCREEN_WIDTH);
}

int Game_AddLocalPlayer(ControlType controlType, int gamepad) {
    if (localPlayersCount >= MAX_LOCAL_PLAYERS) {
        return -1; // too many players
    }

    if (controlType == CONTROL_KEYBOARD_MOUSE) {
        for (int i = 0; i < localPlayersCount; i++) {
            if (localPlayers[i].controlType == CONTROL_KEYBOARD_MOUSE) {
                return -1; // we already have a keyboard/mouse player
            }
        }
    }

    localPlayers[localPlayersCount].player.position = (Vector2){World.spawnX, World.spawnY};
    localPlayers[localPlayersCount].player.animation.texture = currentSkin->texture;
    localPlayers[localPlayersCount].controlType = controlType;

    if (controlType == CONTROL_GAMEPAD) {
        localPlayers[localPlayersCount].gamepad = gamepad;
    }

    localPlayersCount++;

    Game_UpdateWindow();

    return 0;
}

void Game_Init(void) {
    blocksTilemap = FindTilemap("blocks");

    Game_UpdateWindow();
}

void Game_RenderFrame(int player, int x, int y, int w, int h) {
    if (IsWindowResized()) {
        Game_UpdateWindow();
    }
    
    BeginMode2D(localPlayers[player].camera);

    Vector2 leftUpCorner = GetScreenToWorld2D((Vector2){(float)x, (float)y}, localPlayers[player].camera);
    Vector2 rightBottomCorner = GetScreenToWorld2D((Vector2){(float)(x + w), (float)(y + h)}, localPlayers[player].camera);

    leftUpCorner = Vector2Clamp(leftUpCorner, Vector2Zero(), (Vector2){(float)World.width, (float)World.height});
    rightBottomCorner = Vector2Clamp(rightBottomCorner, Vector2Zero(), (Vector2){(float)World.width, (float)World.height});

    int drawnBlocks = 0;

    for (int x = (int)leftUpCorner.x; x < (int)rightBottomCorner.x + 1; x++) {
        for (int y = (int)leftUpCorner.y; y < (int)rightBottomCorner.y + 1; y++) {
            Block block = World.data[PACK_INDEX(x, y, World.width)];

            if (block) {
                DrawTilePro(*blocksTilemap, block-1, (Rectangle){(float)x, (float)y, 1.0f, 1.0f}, Vector2Zero(), 0.0f, WHITE);
                drawnBlocks++;
            }
        }
    }

    for (int i = 0; i < localPlayersCount; i++) {
        Rectangle playerRect = Player_GetVisibleRect(&localPlayers[i].player);
        
        bool playerInScreen = (
            (playerRect.x + playerRect.width > leftUpCorner.x && playerRect.y + playerRect.height > leftUpCorner.y) ||
            (playerRect.x < rightBottomCorner.x && playerRect.y < rightBottomCorner.y)
        );

        if (playerInScreen) {
            Player_Draw(&localPlayers[i].player);
        }
    }

    BlockPosisiton lookAt = localPlayers[player].lookAt;

    DrawRectangleLinesEx((Rectangle){(float)lookAt.x, (float)lookAt.y, 1.0f, 1.0f}, 0.1f, WHITE);

    EndMode2D();

    DrawText(TextFormat("B: %d", drawnBlocks), 0, 20, 20, WHITE);
    DrawText(TextFormat("LUC: [%.02f, %.02f]", leftUpCorner.x, leftUpCorner.y), 0, 40, 20, WHITE);
    DrawText(TextFormat("RDC: [%.02f, %.02f]", rightBottomCorner.x, rightBottomCorner.y), 0, 60, 20, WHITE);
    DrawText(TextFormat("LOOK: [%d, %d]", lookAt.x, lookAt.y), 0, 80, 20, WHITE);
    DrawText(TextFormat("SPEED: [%.02f, %.02f]", localPlayers[0].player.speed.x, localPlayers[0].player.speed.y), 0, 100, 20, WHITE);

    // DrawTilePro(*blocksTilemap, localPlayers[player].player.currentBlock-1, (Rectangle){(float)(GetScreenWidth() - 16), (float)(GetScreenHeight() - 16), 32.0f, 32.0f}, Vector2Zero(), 0.0f, WHITE);

    if (isPaused) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 100});

        int selected = Gui_CenterMenu("Resume;Quit", (Vector2){200.0f, 40.0f});

        switch (selected) {
            case 0: isPaused = false; break;
            case 1: World_Reset(); Gui_ChangeScreen(&MainMenu); isPaused = false; break;
        }
    }
}

void Game_DrawSplitscreen(int player, int x, int y, int w, int h) {
    BeginScissorMode(x, y, w, h);
        Game_RenderFrame(player, x, y, w, h);
    EndScissorMode();
}

void Game_Update(float dt) {
    float alpha = dt / TICKS_PER_SECOND;

    if (!isPaused && World.loaded) {
        for (int i = 0; i < localPlayersCount; i++) {
            LocalPlayer_UpdateControls(&localPlayers[i]);
            localPlayers[i].camera.target = Vector2Lerp(localPlayers[i].player.prevPosition, localPlayers[i].player.position, alpha);
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        isPaused = !isPaused;
    }
}

void Game_OnTick(void) {
    if (!isPaused && World.loaded) {
        for (int i = 0; i < localPlayersCount; i++) Player_Tick(&localPlayers[i].player);
    }
}

void Game_Draw(void) {
    Game_Update(Timer_GetPartialTicks());

    int w = GetScreenWidth();
    int h = GetScreenHeight();

    switch (localPlayersCount) {
        case 1: 
            Game_RenderFrame(0, 0, 0, w, h); 
            break;

        case 2:
            Game_DrawSplitscreen(0, 0, 0, w / 2, h);
            Game_DrawSplitscreen(1, w / 2, 0, w / 2, h);

            DrawLine(w / 2, 0, w / 2, h, BLACK);
            break;
    }
}

Screen GameplayScreen = {
    .Init = Game_Init,
    .Draw = Game_Draw
};