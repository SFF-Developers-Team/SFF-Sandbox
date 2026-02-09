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

SkinMetadata metadataCache[5][512];
int metadataCount[5];

Skin skinCache[1024];
int cachedSkins = 0;

enum SkinPageState {
    PAGE_STATE_DONE,
    PAGE_STATE_LOADING,
    PAGE_STATE_END
} pageState[5] = { 0 };
int currentPage[5] = { 0 };

Skin* FindSkin(int id) {
    for (int i = 0; i < cachedSkins; i++) {
        if (skinCache[i].id == id) return &skinCache[i];
    }

    return NULL;
}

void SkinDownloaded(HttpError error, int id) {
    char* path = GetDataSubdirectory("cache");
    sprintf(path + strlen(path), "/%d.png", id);

    if (FindSkin(id) != NULL) return;

    skinCache[cachedSkins++] = (Skin){id, LoadSkinTexture(path)};
}

void SkinListDownloaded(HttpError error, SkinList* response) {
    if (error != HTTP_NO_ERROR) return;

    for (int i = 0; i < 4; i++) {
        if (response->skins[i].id == 0) {
            pageState[response->type] = PAGE_STATE_END;
            return;
        }

        metadataCache[response->type][metadataCount[response->type]++] = response->skins[i];

        const char* path = GetDataSubdirectory("cache");
        sprintf((char*)(path + strlen(path)), "/%d.png", response->skins[i].id);

        if (!FileExists(path)) {
            Http_DownloadSkin(response->skins[i].id, path, SkinDownloaded);
        } else {
            if (FindSkin(response->skins[i].id) == NULL) skinCache[cachedSkins++] = (Skin){response->skins[i].id, LoadSkinTexture(path)};
        }
    }

    pageState[response->type] = PAGE_STATE_DONE;
}

int viewingId = -1;

void DownloadSkins_Init(void) {
    memset(metadataCache, 0, sizeof(metadataCache));
    memset(metadataCount, 0, sizeof(metadataCount));
    memset(pageState, 0, sizeof(pageState));
    for (int i = 0; i < sizeof(currentPage) / sizeof(int); i++) currentPage[i] = 1;

    viewingId = -1;
}

void DownloadSkins_Draw(void) {
    static int curTab = 0;

    Gui_BeginWindow("BROWSE SKINS");

    if (viewingId <= 0) {
        static bool searching = false;
        static char searchKey[256];

        Gui_Tabs("Featured;New;Best;Search", &curTab);
        Gui_SameLine(2, (float[]){0.8f, 0.2f});

        if (curTab == SKINS_TYPE_SEARCH) {
            Gui_TextInput("search_input", searchKey, sizeof(searchKey));
            if (Gui_Button("Search") && strlen(searchKey)) searching = true;
        }

        float listHeight = Gui_GetRemainingSpace();

        static Rectangle content = { 0 }; 
        static Vector2 scroll = { 0 };
        static Rectangle view = { 0 };
        static Rectangle cell = { 0 };

        bool skinsLoading = curTab != SKINS_TYPE_SEARCH || (curTab == SKINS_TYPE_SEARCH && searching);

        Gui_BeginScrollPanel("skin_browser", listHeight, content, &scroll, &view); 
        {   
            // calculating shit
            // TODO: Review it
            const Vector2 skinSize = {SKIN_FRAME_WIDTH, SKIN_FRAME_HEIGHT}; 
            int skinColumns = 4;
            float skinRows = metadataCount[curTab] / skinColumns;

            float cellW = (view.width - ELEMENT_PADDING * (skinColumns+2)) / skinColumns;
            float cellH = (cellW * skinSize.y) / skinSize.x;
            cell = (Rectangle){view.x, view.y + scroll.y + ELEMENT_PADDING, cellW, cellH};

            content.height = MAX(listHeight, MENU_TEXT_SIZE + skinRows * (cellH + BUTTON_HEIGHT) + (ELEMENT_PADDING * (skinRows+1)));

            for (int i = 0; i < metadataCount[curTab]; i++) {
                // TODO: Gui free draw

                cell.x = view.x + ELEMENT_PADDING * (i % skinColumns + 1) + cellW * (i % skinColumns);

                DrawRectangleLinesEx(cell, 1.f, BLACK);

                Skin* skin = FindSkin(metadataCache[curTab][i].id);

                if (skin != NULL) {
                    int skinW = cellW - ELEMENT_PADDING * 2;
                    int skinH = (skinW * skinSize.y) / skinSize.x;
                    Skin_Draw(skin, (Rectangle){cell.x + ELEMENT_PADDING, cell.y + ELEMENT_PADDING, skinW, skinH});
                }

                if (GuiButton((Rectangle){cell.x, cell.y + cell.height, cell.width, BUTTON_HEIGHT}, "View")) viewingId = i;

                if (i % skinColumns == skinColumns-1) cell.y += cell.height + BUTTON_HEIGHT + ELEMENT_PADDING;
            }

            if (pageState[curTab] == PAGE_STATE_LOADING) {
                GuiLabel((Rectangle){view.x, cell.y, view.width, MENU_TEXT_SIZE}, "Loading skins...");
            }
        }
        Gui_EndScrollPanel();

        bool canLoadNewSkins = cell.y < view.y + view.height && pageState[curTab] == PAGE_STATE_DONE && skinsLoading;

        if (canLoadNewSkins) {
            Http_GetSkins(currentPage[curTab], curTab, searchKey, SkinListDownloaded);

            pageState[curTab] = PAGE_STATE_LOADING;
            currentPage[curTab]++;
        }
    }

    Gui_EndWindow();


    // VIEW SKIN META SCREEN
    //     static int prevMeta = -1;
    //     static struct SkinCached* viewingSkin;
    //     static SkinMetadata* viewingMetadata;
    //     static char dateBuffer[80];
    //     static bool skinInLibrary = false;

    //     if (prevMeta != viewingMeta && viewingMeta >= 0) {
    //         // init this subscreen
    //         viewingSkin = FindSkin(metadataCache[curTab][viewingMeta].id);
    //         viewingMetadata = &metadataCache[viewingMeta];

    //         struct tm *ptm;
    //         time_t date = viewingMetadata->date;

    //         ptm = localtime(&date); 

    //         strftime(dateBuffer, sizeof(dateBuffer), "%d %b, %Y", ptm);

    //         char* path = GetDataSubdirectory("skins");
    //         sprintf(path + strlen(path), "/%s.png", viewingMetadata->name);
    //         skinInLibrary = FileExists(path);
    //     }
    //     prevMeta = viewingMeta;

    //     if (viewingSkin != NULL) {
    //         int tempy = y;
    //         y = baseY + MENU_WINDOW_HEIGHT - ELEMENT_PADDING * 2 - BUTTON_HEIGHT * 2;

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

    // if(GuiButton((Rectangle){x, baseY + MENU_WINDOW_HEIGHT - BUTTON_HEIGHT - ELEMENT_PADDING, w, BUTTON_HEIGHT}, "Back")) {
    //     if (viewingMeta >= 0) {
    //         viewingMeta = -1;
    //     } else {
    //         for (int i = 0; i < cachedSkins; i++) UnloadTexture(skinCache[i].tex);
    //         cachedSkins = 0;
    //         CHANGE_SCREEN(SCREEN_SELECTSKIN);
    //     }
    // }
}