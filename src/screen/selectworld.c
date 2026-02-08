#include "screens.h"
#include "raylib.h"
#include "raygui.h"
#include "files.h"
#include <stdlib.h>
#include <string.h>
#include <world.h>
#include "game.h"

int worldCount;
const char* worldList = NULL;

void SelectWorld_Init(void) {
    if (worldList != NULL) {
        MemFree(worldList);
        worldList = NULL;
    }

    char* saves = GetDataSubdirectory("saves");

    FilePathList list = LoadDirectoryFilesEx(saves, ".dat", false);

    if (list.count == 0) {
        return;
    }
    
    worldCount = list.count;
    worldList = JoinFilePathListFilenames(list);

    UnloadDirectoryFiles(list);
}

void SelectWorld_Draw(void) {
    static int scrollIndex = 0;
    static int selectedWorld = -1;

    Gui_BeginWindow("SELECT WORLD");
    
    Gui_SetPositionMode(FROM_BOTTOM);
    Gui_SameLine(2, NULL);
    if (Gui_Button("Create new world")) Gui_ChangeScreen(&CreateWorld);
    if (Gui_Button("Back") || Gui_PrevScreenCalled()) Gui_ChangeScreen(&MainMenu);

    if (selectedWorld < 0) Gui_Disable();

    Gui_SameLine(2, NULL);
    if (Gui_Button("Play")) {
        int length = 0;
        const char* worldName = GetListElementByIndex(worldList, selectedWorld, &length);

        int result = World_Load(worldName);

        if (result != 0) {
            // SHOW_ERROR("Can\'t load world");
            return;
        }

        Game_AddLocalPlayer(mainControlType, mainGamepad);

        TraceLog(LOG_INFO, "WORLD: Preparing world: %s", worldName);

        Gui_ChangeScreen(&GameplayScreen);
    }

    if (Gui_Button("Delete World")) {
        char* saves = GetDataSubdirectory("saves/");
        int length = 0;
        const char* worldName = GetListElementByIndex(worldList, selectedWorld, &length);
        strncat(saves, worldName, length);
        strcat(saves, ".dat");

        TraceLog(LOG_INFO, "DELETING FILE: %s", saves);
        
        FileRemove(saves);

        SelectWorld_Init();
        selectedWorld = -1;
    }

    Gui_Enable();

    Gui_SetPositionMode(FROM_TOP);
    Gui_ListView(Gui_GetRemainingSpace(), worldList, &scrollIndex, &selectedWorld);

    Gui_EndWindow();
}

Screen SelectWorld = {
    .Init = SelectWorld_Init,
    .Draw = SelectWorld_Draw
};