#include "screens.h"
#include "raygui.h"
#include "raymath.h"
#include <stddef.h> // NULL

void MainMenu_Draw(void) {
    int selected = Gui_CenterMenu("Play;Skins;Quit", (Vector2){200.0f, 40.0f});

    switch (selected) {
        case 0: Gui_ChangeScreen(&SelectWorld); break;
        case 1: Gui_ChangeScreen(&SelectSkin); break;
        case 2: CloseWindow();
    }
}

Screen MainMenu = {
    .Init = NULL, 
    .Draw = MainMenu_Draw
};
