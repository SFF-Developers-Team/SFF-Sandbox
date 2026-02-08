#include "screens.h"
#include "raylib.h"
#include "raygui.h"
#include "animation.h"
#include "textures.h"
#include <stdlib.h>
#include "files.h"
#include <stdio.h>
#include "skin.h"
#include <string.h>

Texture2D* skins = NULL;
const char* skinNames = NULL;
int selectedSkin = 0;
int loadedSkins = 0;

void SelectSkin_Init(void) {
    if (skinNames != NULL) {
        MemFree(skinNames);
        skinNames = NULL;
    }

    char* dir = GetDataSubdirectory("skins");

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

int DrawSkinFrame(Rectangle bounds, const char* label) {
    DrawRectangleLinesEx(bounds, 1.f, WHITE); // TODO: Do a gui function for color-themed rectangle draw

    int skinH = bounds.height - ELEMENT_PADDING * 2;
    int skinW = SKIN_FRAME_WIDTH * (skinH / SKIN_FRAME_HEIGHT);

    Skin_Draw((loadedSkins > 0)? &(Skin){0,  skins[selectedSkin]} : defaultSkin, (Rectangle){bounds.x + (bounds.width - skinW) / 2, bounds.y + ELEMENT_PADDING, skinW, skinH});

    return 0;
}

void SelectSkin_Draw(void) {
    Gui_BeginWindow("SELECT SKIN");

    Gui_SetPositionMode(FROM_BOTTOM);
    Gui_SameLine(2, NULL);
    if (Gui_Button("Delete skin")) {
        char* path = GetDataSubdirectory("skins/");
        int length = 0;
        const char* skinName = GetListElementByIndex(skinNames, selectedSkin, &length);
        strncat(path, skinName, length);
        strcat(path, ".png");
        
        FileRemove(path);

        SelectSkin_Init();
        selectedSkin = -1;
    }

    if (Gui_Button("Back") || Gui_IsNavBack()) {
        // TODO: Make this work
        // currentSkin->texture = skins[selectedSkin];
        Gui_ChangeScreen(&MainMenu);
        SkinSelect_Deinit();
    }

    Gui_SameLine(2, NULL);
    Gui_Button("Download more skins");

    if (Gui_Button("Open skins dir")) OpenURL(GetDataSubdirectory("skins"));
    Gui_ComboBox((loadedSkins > 0)? skinNames : "Default", &selectedSkin);

    Gui_CustomElement(Gui_GetRemainingSpace(), false, NULL, DrawSkinFrame);
    Gui_EndWindow();

    if (IsFileDropped()) {
        FilePathList list = LoadDroppedFiles();

        const char* skins = GetDataSubdirectory("skins");

        for (int i = 0; i < list.count; i++) {
            if (IsFileExtension(list.paths[i], ".png")) {
                FileCopy(list.paths[i], skins);
            }
        }

        UnloadDroppedFiles(list);
        SelectSkin_Init();
    }
}

Screen SelectSkin = {
    SelectSkin_Init,
    SelectSkin_Draw
};