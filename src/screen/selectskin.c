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

Skin* skins = NULL;
const char* names = NULL;

int selected = 0;
int amount = 0;

void SelectSkin_Init(void) {
    char* dir = GetDataSubdirectory("skins");

    FilePathList list = LoadDirectoryFilesEx(dir, ".png", false);

    if (list.count > 0) {
        skins = MemAlloc(list.count * sizeof(Texture2D));

        for (int i = 0; i < list.count; i++) {
            int scaned = sscanf(list.paths[i], "%i_", &skins[i].id);
            skins[i].texture = LoadSkinTexture(list.paths[i]);

            if (skins[i].texture.id) {
                amount++;
            }
        }
    
        names = JoinFilePathListFilenames(list);
    }

    UnloadDirectoryFiles(list);

    char* full = MemAlloc(TextLength(names) + 8);
    strcpy(full, "Default;");
    strcat(full, names);

    MemFree(names);

    names = full;
}

void SkinSelect_Deinit(void) {
    for (int i = 0; i < amount; i++) {
        if (i == selected-1) continue;
        UnloadTexture(skins[i].texture);
    }

    if (skins != NULL) MemFree(skins);
    skins = NULL;

    if (names != NULL) MemFree(names);
    names = NULL;

    amount = 0;
}

void SelectSkin_Draw(void) {
    Gui_BeginWindow("SELECT SKIN");

    Gui_SetPositionMode(FROM_BOTTOM);
    Gui_SameLine(2, NULL);
    if (Gui_Button("Delete skin")) {
        char* path = GetDataSubdirectory("skins/");
        int length = 0;
        const char* skinName = GetListElementByIndex(names, selected, &length);
        strncat(path, skinName, length);
        strcat(path, ".png");
        
        FileRemove(path);

        SelectSkin_Init();
        selected = -1;
    }

    if (Gui_Button("Back") || Gui_IsNavBack()) {
        static Skin selectedSkin;
        selectedSkin = skins[selected-1];

        currentSkin = &selectedSkin;

        SkinSelect_Deinit();
        Gui_ChangeScreen(&MainMenu);
    }

    Gui_SameLine(2, NULL);
    if (Gui_Button("Download more skins")) Gui_ChangeScreen(&DownloadSkins);

    if (Gui_Button("Open skins dir")) OpenURL(GetDataSubdirectory("skins"));
    Gui_ComboBox((amount > 0)? names : "Default", &selected);

    Gui_SkinSlot((selected > 0)? &skins[selected-1] : &defaultSkin, Gui_GetRemainingSpace());
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