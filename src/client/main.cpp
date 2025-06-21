#include <raylib.h>

int main() {
    InitWindow(1280, 720, "SFF Sandbox");

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Hello world!", 1, 1, 100, GRAY);
        EndDrawing();
    }

    CloseWindow();
}