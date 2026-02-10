#include "screens.h"
#include "httpclient.h"
#include <stddef.h> // for NULL
#include "raygui.h"
#include <string.h>
#include <files.h>
#include <stdio.h>
#include "animation.h"
#include "utils.h"
#include "skin.h"
#include <time.h>

Skin skinCache[1024];
int cachedSkins = 0;

const char* state;

enum SkinPageState {
    PAGE_STATE_DONE,
    PAGE_STATE_LOADING,
    PAGE_STATE_END
};

struct CategoryState {
    enum SkinPageState pageState;
    int curPage;

    SkinMetadata metadataCache[512];
    int cacheSize;
} categoryState[SKINS_TYPE_COUNT] = { 0 };

Skin* FindSkin(int id) {
    for (int i = 0; i < cachedSkins; i++) {
        if (skinCache[i].id == id) return &skinCache[i];
    }

    return NULL;
}

void SkinDownloaded(int error, int id) {
    if (error != 0) {
        TraceLog(LOG_ERROR, "Could not download skin %i: %s", id, Http_Error(error));
        return;
    }

    char* path = GetDataSubdirectory("cache");
    sprintf(path + strlen(path), "/%d.png", id);

    if (FindSkin(id) != NULL) return;

    skinCache[cachedSkins++] = (Skin){id, LoadSkinTexture(path)};
}

void SkinListDownloaded(int error, SkinList* response) {
    if (error != 0) {
        state = "Error";
        TraceLog(LOG_ERROR, "Could not download skin list: %s", Http_Error(error));
        return;
    }

    TraceLog(LOG_DEBUG, "Skin list downloaded page: %i", response->page);

    for (int i = 0; i < 4; i++) {
        if (response->skins[i].id == 0) {
            categoryState[response->type].pageState = PAGE_STATE_END;
            state = NULL;
            return;
        }

        categoryState[response->type].metadataCache[categoryState[response->type].cacheSize++] = response->skins[i];

        const char* path = GetDataSubdirectory("cache");
        sprintf((char*)(path + strlen(path)), "/%d.png", response->skins[i].id);

        if (!FileExists(path)) {
            Http_DownloadSkin(response->skins[i].id, path, SkinDownloaded);
        } else {
            if (FindSkin(response->skins[i].id) == NULL) skinCache[cachedSkins++] = (Skin){response->skins[i].id, LoadSkinTexture(path)};
        }
    }

    categoryState[response->type].pageState = PAGE_STATE_DONE;
    state = NULL;
}

int viewingId = -1;

void DownloadSkins_Init(void) {
    memset(categoryState, 0, sizeof(categoryState));
    for (int i = 0; i < sizeof(categoryState) / sizeof(struct CategoryState); i++) categoryState[i].curPage = 1;

    viewingId = -1;
}

void DownloadSkins_Draw(void) {
    static int curTab = 0;

    Gui_BeginWindow("BROWSE SKINS");

    Gui_SetPositionMode(FROM_BOTTOM);
    if(Gui_Button("Back")) {
        if (viewingId <= 0) {
            for (int i = 0; i < cachedSkins; i++) UnloadTexture(skinCache[i].texture);
            cachedSkins = 0;
            Gui_ChangeScreen(&SelectSkin);
        } else 
            viewingId = -1;
    }
    Gui_SetPositionMode(FROM_TOP);

    if (viewingId <= 0) {
        static bool searching = false;
        static char searchKey[256];

        Gui_Tabs("Featured;New;Best;Search", &curTab);
       
        if (curTab == SKINS_TYPE_SEARCH) {
            Gui_SameLine(2, (float[]){0.8f, 0.2f});
            Gui_TextInput("search_input", searchKey, sizeof(searchKey));
            if (Gui_Button("Search") && strlen(searchKey)) searching = true;
        }

        float listHeight = Gui_GetRemainingSpace();

        static Rectangle content = { 0 }; 
        static Vector2 scroll = { 0 };
        static Rectangle view = { 0 };

        bool skinsLoading = curTab != SKINS_TYPE_SEARCH || (curTab == SKINS_TYPE_SEARCH && searching);

        Gui_BeginScrollPanel("skin_browser", listHeight, content, &scroll, &view); 
        {   
            const Vector2 skinSize = {SKIN_FRAME_WIDTH, SKIN_FRAME_HEIGHT}; 
            int skinColumns = 4;
            int skinRows = categoryState[curTab].cacheSize / skinColumns;

            float cellW = view.width / skinColumns;
            float cellH = (cellW * skinSize.y) / skinSize.x;

            for (int i = 0; i < categoryState[curTab].cacheSize / skinColumns; i++) {
                Gui_SameLine(skinColumns, NULL);

                for (int j = 0; j < categoryState[curTab].cacheSize % skinColumns; j++) {
                    int index = i * skinColumns + j;

                    if (index >= categoryState[curTab].cacheSize) break;

                    Skin* skin = FindSkin(categoryState[curTab].metadataCache[index].id);
                    Gui_SkinSlot(skin, cellH);
                }

                Gui_SameLine(skinColumns, NULL);

                static char viewBtnName[16] = {"View##"};

                for (int j = 0; j < categoryState[curTab].cacheSize % skinColumns; j++) {
                    int index = i * skinColumns + j;

                    if (index >= categoryState[curTab].cacheSize) break;

                    snprintf(viewBtnName + 8, sizeof(viewBtnName) - 8, "%i", categoryState[curTab].metadataCache[index].id);
                    Gui_Button(viewBtnName);
                }
            }

            Gui_SameLine(1, NULL);


            //     cell.x = view.x + ELEMENT_PADDING * (i % skinColumns + 1) + cellW * (i % skinColumns);

            //     DrawRectangleLinesEx(cell, 1.f, BLACK);

            //     Skin* skin = FindSkin(categoryState[curTab].metadataCache[i].id);

            //     if (skin != NULL) {
            //         int skinW = cellW - ELEMENT_PADDING * 2;
            //         int skinH = (skinW * skinSize.y) / skinSize.x;
            //         Skin_Draw(skin, (Rectangle){cell.x + ELEMENT_PADDING, cell.y + ELEMENT_PADDING, skinW, skinH});
            //     }

            //     if (GuiButton((Rectangle){cell.x, cell.y + cell.height, cell.width, BUTTON_HEIGHT}, "View")) viewingId = i;

            //     if (i % skinColumns == skinColumns-1) cell.y += cell.height + BUTTON_HEIGHT + ELEMENT_PADDING;
            // }

            // if (state) {
            //     GuiLabel((Rectangle){view.x, view.y, view.width, MENU_TEXT_SIZE}, state);
            // }

            Rectangle last = Gui_GetLastBounds();

            content.height = MAX(listHeight, last.y);

            bool canLoadNewSkins = last.y < view.y + view.height && categoryState[curTab].pageState == PAGE_STATE_DONE && skinsLoading;

            if (canLoadNewSkins) {
                Http_GetSkins(categoryState[curTab].curPage, curTab, searchKey, SkinListDownloaded);

                categoryState[curTab].pageState = PAGE_STATE_LOADING;
                categoryState[curTab].curPage++;

                state = "Loading skins...";
            }
        }
        Gui_EndScrollPanel();
    } // else {
    //     static int prevMeta = -1;
    //     static struct Skin* viewingSkin;
    //     static SkinMetadata* viewingMetadata;
    //     static char dateBuffer[80];
    //     static bool skinInLibrary = false;

    //     if (prevMeta != viewingId && viewingId >= 0) {
    //         // init this subscreen
    //         viewingSkin = FindSkin(categoryState[curTab].metadataCache[viewingId].id);
    //         viewingMetadata = &categoryState[curTab].metadataCache[viewingId];

    //         struct tm *ptm;
    //         time_t date = viewingMetadata->date;

    //         ptm = localtime(&date); 

    //         strftime(dateBuffer, sizeof(dateBuffer), "%d %b, %Y", ptm);

    //         char* path = GetDataSubdirectory("skins");
    //         sprintf(path + strlen(path), "/%s.png", viewingMetadata->name);
    //         skinInLibrary = FileExists(path);
    //     }
    //     prevMeta = viewingId;

    //     if (viewingSkin != NULL) {
    //         if (skinInLibrary) GuiSetState(STATE_DISABLED);

    //         if (GuiButton((Rectangle){x, y, w, BUTTON_HEIGHT}, "Add to my skins")) {
    //             char src[MAX_PATH];
    //             char* path = GetDataSubdirectory("cache");
    //             strcpy(src, path);
    //             sprintf(src + strlen(src), "/%d.png", viewingMetadata->id);

    //             path = GetDataSubdirectory("skins");
    //             sprintf(path + strlen(path), "/%s.png", viewingMetadata->name);
                
    //             FileCopy(src, path);

    //             skinInLibrary = true;
    //         }
    //         GuiSetState(STATE_NORMAL);
    //         y -= BUTTON_HEIGHT + ELEMENT_PADDING + MENU_TEXT_SIZE * 3;

    //         GuiLabel((Rectangle){x, y + MENU_TEXT_SIZE * 0, w, MENU_TEXT_SIZE}, TextFormat("Skin name: %s", viewingMetadata->name));
    //         GuiLabel((Rectangle){x, y + MENU_TEXT_SIZE * 1, w, MENU_TEXT_SIZE}, TextFormat("Author: %s", viewingMetadata->author));
    //         GuiLabel((Rectangle){x, y + MENU_TEXT_SIZE * 2, w, MENU_TEXT_SIZE}, TextFormat("Skin ID: %d", viewingMetadata->id));
    //         GuiLabel((Rectangle){x, y + MENU_TEXT_SIZE * 3, w, MENU_TEXT_SIZE}, TextFormat("Upload date: %s", dateBuffer));

    //         y -= BUTTON_HEIGHT + ELEMENT_PADDING;

    //         static EntityAnimationType currentType;
    //         static char currentAnim[32] = "Animation: Idle";
    //         if (GuiButton((Rectangle){x, y, w, BUTTON_HEIGHT}, currentAnim)) {
    //             currentType++;
    //             if (currentType > ENTITY_CART) currentType = 0;

    //             const char* names[] = {"Idle", "Walk", "Sneak", "Jump", "Hit", "Hurt", "Sit", "Cart"};

    //             sprintf(currentAnim, "Animation: %s", names[currentType]);
    //         }

    //         int skinH = y - tempy - ELEMENT_PADDING * 2;
    //         int skinW = SKIN_FRAME_WIDTH * (skinH / SKIN_FRAME_HEIGHT);

    //         EntityAnimation_DrawDummyPlayer(currentType, (Rectangle){x + (w - skinW) / 2, tempy, skinW, skinH}, viewingSkin->tex.id);
    //     }
    // }

    Gui_EndWindow();
}

Screen DownloadSkins = {
    DownloadSkins_Init,
    DownloadSkins_Draw
};