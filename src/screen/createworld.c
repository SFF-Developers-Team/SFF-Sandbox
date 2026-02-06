#include "screens.h"
#include "raylib.h"
#include "world.h"
#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "error.h"

#define LISTEN_CHANGES_INT(var)     \
    static int prev##var;           \
    bool changed##var = prev##var != var;\
    prev##var = var;                \
    if (changed##var)                    \

#define LISTEN_CHANGES_STR(var)             \
    static int prev##var;                   \
    int size##var = strlen(var);            \
    bool changed##var = prev##var != size##var;  \
    prev##var = size##var;                  \
    if (changed##var) 


static const char *prefixes[] = {
    "Ash", "Stone", "Iron", "Red", "Black",
    "Cold", "Old", "Deep", "Lost", "Grave",
    "Dust", "Frost", "Dark", "Wild"
};

static const char *suffixes[] = {
    "vale", "reach", "hollow", "mere", "scar",
    "wood", "fall", "root", "field", "ridge",
    "depths", "plain"
};

void CreateWorld_Draw(void) {
    static char seed[64] = { 0 };
    static int type;
    static int size;
    static Texture2D preview = { 0 };
    static bool createLocked = true;
    static char worldName[MAX_WORLD_NAME] = { 0 };

    Gui_BeginWindow("CREATE WORLD");
    
    static float ratios[] = {0.8f, 0.2f};
    Gui_SameLine(2, ratios);
    Gui_LabelNextElement("World name:");
    Gui_TextInput("world_name", worldName, sizeof(worldName));
    if (Gui_Button("RAND##1")) {
        int prefix = GetRandomValue(0, (sizeof(prefixes)/sizeof(*prefixes))-1);
        int suffix = GetRandomValue(0, (sizeof(suffixes)/sizeof(*suffixes))-1);

        const char *p = prefixes[prefix];
        const char *s = suffixes[suffix];

        snprintf(worldName, sizeof(worldName), "%s %s", p, s);
    }

    Gui_SameLine(2, ratios);
    Gui_LabelNextElement("World seed:");
    Gui_TextInput("world_seed", seed, sizeof(seed));
    if (Gui_Button("RAND##2")) {
        snprintf(seed, sizeof(seed), "%d", rand());
    }

    Gui_LabelNextElement("World type:");
    Gui_ComboBox("Default;Flat", &type);

    Gui_LabelNextElement("World size:");
    Gui_ComboBox("Small;Medium;Large", &size);

    if (createLocked) Gui_Disable();
    Gui_SetPositionMode(FROM_BOTTOM);
    Gui_SameLine(2, NULL);
    if (Gui_Button("Create new world")) {
        // Init world data
        World.size = size;
        World.type = type;
        World.seed = ComputeCRC32(seed, strlen(seed));

        int result = World_Generate(World.type, World.size, World.seed);
            
        if (result == RES_OUT_OF_MEMORY) {
            // SHOW_ERROR("Out of memory. Try load or join to another world.");
            return;
        }
        
        World.name = worldName;
        result = World_Save();

        if (result) {
            // SHOW_ERROR("Can\'t save world. (idk why)");
            return;
        }

        Game_AddLocalPlayer(mainControlType, mainGamepad);

        Gui_ChangeScreen(&GameplayScreen);
    }
    Gui_Enable();
    if (Gui_Button("Back") || Gui_PrevScreenCalled()) Gui_ChangeScreen(&SelectWorld);

    Gui_SetPositionMode(FROM_TOP);
    Gui_LabelNextElement("Preview:");
    Gui_Texture(&preview, NULL, Gui_GetRemainingSpace());

    Gui_EndWindow();

    static bool shouldUpdatePreview = true;

    createLocked = !strlen(worldName);
    
    LISTEN_CHANGES_STR(seed) shouldUpdatePreview = true;
    LISTEN_CHANGES_INT(type) shouldUpdatePreview = true;
    LISTEN_CHANGES_INT(size) shouldUpdatePreview = true;

    if (shouldUpdatePreview) {
        preview = World_GeneratePreview((int)ComputeCRC32(seed, strlen(seed)), type, size);
        shouldUpdatePreview = false;
    }
}

Screen CreateWorld = {
    .Init = NULL,
    .Draw = CreateWorld_Draw
};