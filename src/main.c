#include "raylib.h"
#include "raymath.h"
#include "world.h"
#include "tilemap.h"
#include "player.h"
#include "collisions.h"
#include "defines.h"
#include "utils.h"
#include "rlgl.h"
#include "screens.h"
#include "files.h"
#include "game.h"
#include "textures.h"
#include "raygui.h"
#include <stdlib.h>
#include <time.h>
#include "httpclient.h"
#include "skin.h"
#include "timer.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const char* error = NULL;
ControlType mainControlType;
int mainGamepad = -1;

Skin defaultSkin;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame
void InitGame(void);            // Init game

//----------------------------------------------------------------------------------
// Program main entry point
//----------------------------------------------------------------------------------
int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(1280, 720, "sandbox");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);

    InitGame();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {   // Detect window close button or ESC key
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void InitGame(void) {
    InitFiles();

    static struct TextureEntry blocks = {"blocks.png"};
    static struct TextureEntry player = {"player.png"};

    LoadTextureEntry(&blocks);
    LoadTextureEntry(&player);

    static struct TilemapEntry tlBlocks;
    tlBlocks = (struct TilemapEntry){"blocks", (Tilemap){blocks.texture, 16, 16}};

    LoadTilemapEntry(&tlBlocks);

    defaultSkin = (Skin){0, player.texture};
    currentSkin = &defaultSkin;

    SetRandomSeed((unsigned int)time(0));

    Gui_Init();
    Game_Init();
    Http_Init();
}

void UpdateDrawFrame(void) {
    // Update
    //----------------------------------------------------------------------------------
    if (IsKeyPressed(KEY_F11)) {
        ToggleBorderlessWindowed();
    }

    Timer_Update();
    Http_Update();

    if (GetGamepadButtonPressed() > 0) {
        mainControlType = CONTROL_GAMEPAD;
        mainGamepad = 0;
    }

    Vector2 delta = GetMouseDelta();
    if (delta.x != 0 || delta.y != 0) {
        mainControlType = CONTROL_KEYBOARD_MOUSE;
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
        ClearBackground(SKYBLUE);

        if (currentScreen != NULL) {
            currentScreen->Draw();
        }

        DrawFPS(0, 0);

    EndDrawing();
    //----------------------------------------------------------------------------------
}
