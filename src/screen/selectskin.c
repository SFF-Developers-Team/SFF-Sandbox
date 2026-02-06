#include "screens.h"
#include "raylib.h"
#include "raygui.h"
#include "animation.h"
#include "textures.h"
#include <stdlib.h>
#include "files.h"
#include <stdio.h>
#include "skin.h"

Texture2D* skins = NULL;
const char* skinNames = NULL;
int selectedSkin = 0;
int loadedSkins = 0;

void SelectSkin_Init(void) {
    if (skinNames != NULL) {
        MemFree(skinNames);
        skinNames = NULL;
    }

    const char* dir = GetDataSubdirectory("skins");

    FilePathList list = LoadDirectoryFilesEx(dir, ".png", false);
    loadedSkins = list.count;

    if (list.count == 0) {
        return;
    }

    if (skins == NULL) {
        skins = MemAlloc((list.count + 1) * sizeof(Texture2D));
    }

    skins[0] = defaultSkin->texture;

    for (int i = 0; i < loadedSkins; i++) {
        skins[i+1] = LoadTexture(list.paths[i]);
    }
    
    skinNames = JoinFilePathListFilenames(list);

    UnloadDirectoryFiles(list);

    // TODO: review this
    // I had to do this due to TextInsert function don't work
    char* temp[] = {"Default", skinNames};
    const char* full = TextJoin(temp, 2, ";");
    MemFree(skinNames);
    skinNames = MemAlloc(TextLength(full));
    TextCopy(skinNames, full);
}

void SkinSelect_Deinit(void) {
    for (int i = 0; i < loadedSkins; i++) {
        if (i == selectedSkin) continue;
        UnloadTexture(skins[i+1]);
    }

    if (skins != NULL) MemFree(skins);
    skins = NULL;

    if (skinNames != NULL) MemFree(skinNames);
    skinNames = NULL;

    loadedSkins = 0;
}

void SelectSkin_Draw(void) {
    // int x, y, w, baseY;
    // Gui_WorldScreenBase("SELECT SKIN", &x, &y, &w, &baseY); 

    // int tempy = y;
    // y = baseY + MENU_WINDOW_HEIGHT - (BUTTON_HEIGHT + ELEMENT_PADDING) * 3;

    // int skinH = y - tempy - ELEMENT_PADDING * 2;
    // int skinW = SKIN_FRAME_WIDTH * (skinH / SKIN_FRAME_HEIGHT);

    // EntityAnimation_DrawDummyPlayer(ENTITY_IDLE, (Rectangle){x + (w - skinW) / 2, tempy, skinW, skinH}, (loadedSkins > 0)? skins[selectedSkin].id : 0);

    // GuiComboBox((Rectangle){x, y, w, INPUT_HEIGHT}, (loadedSkins > 0)? skinNames : "Default", &selectedSkin);
    // NEXT_ELEMENT(y, INPUT_HEIGHT);

    // int half = w / 2 - ELEMENT_PADDING;

    // if (GuiButton((Rectangle){x, y, half, BUTTON_HEIGHT}, "Download more skins")) {
    //     CHANGE_SCREEN(SCREEN_DOWNLOADSKINS);
    // }
    
    // if (GuiButton((Rectangle){x + half + ELEMENT_PADDING, y, half + ELEMENT_PADDING, BUTTON_HEIGHT}, "Open skins dir")) {
    //     OpenURL(GetDataSubdirectory("skins"));
    // }

    // NEXT_ELEMENT(y, INPUT_HEIGHT);

    // if (GuiButton((Rectangle){x, y, half, BUTTON_HEIGHT}, "Back")) {
    //     currentSkin = skins[selectedSkin];
    //     CHANGE_SCREEN(SCREEN_MAINMENU);
    //     SkinSelect_Deinit();
    //     return;
    // }
    // if (GuiButton((Rectangle){x + half + ELEMENT_PADDING, y, half + ELEMENT_PADDING, BUTTON_HEIGHT}, "Delete skin")) {
    //     const char* path = GetDataSubdirectory("skins/");
    //     int length = 0;
    //     const char* skinName = GetListElementByIndex(skinNames, selectedSkin, &length);
    //     strncat(path, skinName, length);
    //     strcat(path, ".png");
        
    //     FileRemove(path);

    //     SelectSkin_Init();
    //     selectedSkin = -1;
    // }

    // if (IsFileDropped()) {
    //     FilePathList list = LoadDroppedFiles();

    //     const char* skins = GetDataSubdirectory("skins");

    //     for (int i = 0; i < list.count; i++) {
    //         if (IsFileExtension(list.paths[i], ".png")) {
    //             FileCopy(list.paths[i], skins);
    //         }
    //     }

    //     UnloadDroppedFiles(list);
    //     SelectSkin_Init();
    // }
}