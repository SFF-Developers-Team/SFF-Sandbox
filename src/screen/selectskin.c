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

void SkinSelect_Deinit(void) {
    if (skins != NULL) {
        for (int i = 0; i < amount; i++) {
            if (i == selected-1) continue;
            UnloadTexture(skins[i].texture);
        }

        MemFree(skins);
        skins = NULL;
    }

    if (names != NULL) {
        MemFree((void*)names);
        names = NULL;
    }

    amount = 0;
}


void SelectSkin_Init(void) {
    if (skins || names) SkinSelect_Deinit();

    char* dir = GetDataSubdirectory("skins");

    FilePathList list = LoadDirectoryFilesEx(dir, ".png", false);

    if (list.count > 0) {
        skins = MemAlloc(list.count * sizeof(Texture2D));

        int valid = 0;

        for (int i = 0; i < list.count; i++) {
            Texture2D tex = LoadSkinTexture(list.paths[i]);
            if (tex.id) {
                int id;

                if (sscanf(list.paths[i], "%i_", &id) == 1)
                    skins[valid].id = id;
                else
                    skins[valid].id = 0;

                skins[valid].texture = tex;
                valid++;
            }
        }

        amount = valid;
        names = JoinFilePathListFilenames(list);
    }

    UnloadDirectoryFiles(list);

    int namesLen = names ? TextLength(names) : 0;

    char* full = MemAlloc(namesLen + TextLength("Default;") + 1);

    strcpy(full, "Default;");
    if (names) strcat(full, names);


    MemFree((void*)names);

    names = full;
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