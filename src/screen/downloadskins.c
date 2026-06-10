#include "screens.h"
#include "httpclient.h"
#include <stddef.h> // for NULL
#include <string.h>
#include <files.h>
#include <stdio.h>
#include "animation.h"
#include "utils.h"
#include "skin.h"
#include <time.h>

// SKIN VIEW
struct SkinViewContext {
    SkinMetadata* meta;
    Skin* skin;

    bool isInLibrary;

    char dateStr[80];
} ViewCtx;

char* DownloadSkins_GetSkinPath(SkinDownloadPath type, SkinMetadata* meta) {
    return GetDataSubdirectory(TextFormat("%s/%d_%s.png", (type == PATH_CACHE)? "cache" : "skins", ViewCtx.meta->id, ViewCtx.meta->name));
}

void DownloadSkins_InitView(SkinMetadata* meta, Skin* skin) {
    ViewCtx.meta = meta;
    ViewCtx.skin = skin;

    char* path = DownloadSkins_GetSkinPath(PATH_SKINS, meta);
    ViewCtx.isInLibrary = FileExists(path);

    time_t time = meta->date;
    struct tm* ptm = localtime(&time); 
    strftime(ViewCtx.dateStr, sizeof(ViewCtx.dateStr), "%d %b, %Y", ptm);
}

void DownloadSkins_DeinitView() {
    ViewCtx.meta = NULL;
    ViewCtx.skin = NULL;
}

void DownloadSkins_DrawView() {
    if (ViewCtx.isInLibrary) {
        Gui_Disable();
    }

    if (Gui_Button("Add to my skins")) {
        char src[MAX_PATH];
        char* path = GetDataSubdirectory(TextFormat("cache/%d.png", ViewCtx.meta->id));
        strcpy(src, path);
        
        path = DownloadSkins_GetSkinPath(PATH_SKINS, ViewCtx.meta);
  
        FileCopy(src, path);

        ViewCtx.isInLibrary = true;
    }

    Gui_Enable();

    Gui_Label(TextFormat("Upload date: %s", ViewCtx.dateStr));
    Gui_Label(TextFormat("Skin ID: %d", ViewCtx.meta->id));
    Gui_Label(TextFormat("Author: %s", ViewCtx.meta->author));
    Gui_Label(TextFormat("Skin name: %s", ViewCtx.meta->name));

    Gui_SetPositionMode(FROM_TOP);

    Gui_SkinSlot(ViewCtx.skin, Gui_GetRemainingSpace());

    //     static EntityAnimationType currentType;
    //     static char currentAnim[32] = "Animation: Idle";
    //     if (GuiButton((Rectangle){x, y, w, BUTTON_HEIGHT}, currentAnim)) {
    //         currentType++;
    //         if (currentType > ENTITY_CART) currentType = 0;

    //         const char* names[] = {"Idle", "Walk", "Sneak", "Jump", "Hit", "Hurt", "Sit", "Cart"};

    //         sprintf(currentAnim, "Animation: %s", names[currentType]);
    //     }

    //     int skinH = y - tempy - ELEMENT_PADDING * 2;
    //     int skinW = SKIN_FRAME_WIDTH * (skinH / SKIN_FRAME_HEIGHT);

    //     EntityAnimation_DrawDummyPlayer(currentType, (Rectangle){x + (w - skinW) / 2, tempy, skinW, skinH}, viewingSkin->tex.id);
    // }
}


// SKIN CATALOG
enum SkinPageState {
    PAGE_STATE_DONE,
    PAGE_STATE_LOADING,
    PAGE_STATE_END
};

struct SkinCatalogContext {
    const char* state; // "Loading skins..." / "Error!"
    SkinsType tab; // current tab in catalog

    struct {
        Skin skins[1024];
        int count;
    } skinCache;
  
    struct CategoryState {
        /*
        * PAGE_STATE_DONE page loaded but there is more
        * PAGE_STATE_LOADING loading page
        * PAGE_STATE_END there is no more pages to load
        */
        enum SkinPageState state;
        int current; // current page on mineblocks API

        struct {
            SkinMetadata metadata[512];
            int count;
        } metaCache;
    } pagesCache[SKINS_TYPE_COUNT];
} CatalogCtx;

static inline void SkinCatalog_AddMeta(SkinsType type, SkinMetadata meta) {
    // TODO: Make it dynamic and safe

    CatalogCtx.pagesCache[type].metaCache.metadata[CatalogCtx.pagesCache[type].metaCache.count++] = meta;
}

static inline void SkinCatalog_AddSkin(Skin skin) {
    CatalogCtx.skinCache.skins[CatalogCtx.skinCache.count++] = skin;
}

Skin* FindSkin(int id) {
    for (int i = 0; i < CatalogCtx.skinCache.count; i++) {
        if (CatalogCtx.skinCache.skins[i].id == id) return &CatalogCtx.skinCache.skins[i];
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

    CatalogCtx.skinCache.skins[CatalogCtx.skinCache.count++] = (Skin){id, LoadSkinTexture(path)};
}

void SkinListDownloaded(int error, SkinList* response) {
    if (error != 0) {
        CatalogCtx.state = "Error";
        TraceLog(LOG_ERROR, "Could not download skin list: %s", Http_Error(error));
        return;
    }

    TraceLog(LOG_DEBUG, "Skin list downloaded page: %i", response->page);

    for (int i = 0; i < 4; i++) {
        if (response->skins[i].id == 0) {
            CatalogCtx.pagesCache[response->type].state = PAGE_STATE_END;
            CatalogCtx.state = NULL;
            return;
        }

        SkinCatalog_AddMeta(response->type, response->skins[i]);

        char* path = GetDataSubdirectory("cache");
        sprintf(&path[strlen(path)], "/%d.png", response->skins[i].id);

        if (!FileExists(path)) {
            Http_DownloadSkin(response->skins[i].id, PATH_CACHE, SkinDownloaded);
        } else {
            if (FindSkin(response->skins[i].id) == NULL) SkinCatalog_AddSkin((Skin){response->skins[i].id, LoadSkinTexture(path)});
        }
    }

    CatalogCtx.pagesCache[response->type].state = PAGE_STATE_DONE;
    CatalogCtx.state = NULL;
}

void DownloadSkin_CatalogDraw() {
    static char searchKey[256];
    
    Gui_SetPositionMode(FROM_TOP);
    Gui_Tabs("Featured;New;Best;Search", (int*)&CatalogCtx.tab);
    
    if (CatalogCtx.tab == SKINS_TYPE_SEARCH) {
        Gui_SameLine(2, (float[]){0.8f, 0.2f});
        Gui_TextInput("search_input", searchKey, sizeof(searchKey));

        // Activate search
        if (Gui_Button("Search") && strlen(searchKey) > 0) CatalogCtx.pagesCache[SKINS_TYPE_SEARCH].state = PAGE_STATE_DONE;
    }

    int tab = CatalogCtx.tab;

    float listHeight = Gui_GetRemainingSpace();

    static Rectangle content = { 0 }; 
    static Vector2 scroll = { 0 };
    static Rectangle view = { 0 };

    struct CategoryState* cat = &CatalogCtx.pagesCache[tab];

    content.height += ELEMENT_PADDING;

    Gui_BeginScrollPanel("skin_browser", listHeight, content, &scroll, &view); 
    {   
        const Vector2 skinSize = {SKIN_FRAME_WIDTH, SKIN_FRAME_HEIGHT};
        
        int skinCount = CatalogCtx.pagesCache[tab].metaCache.count;
        int skinColumns = 4;
        int skinRows = skinCount / skinColumns;

        float cellW = view.width / skinColumns;
        float cellH = (cellW * skinSize.y) / skinSize.x;

        for (int i = 0; i < skinRows; i++) {
            Gui_SameLine(skinColumns, NULL);

            for (int j = 0; j < skinColumns; j++) {
                int index = i * skinColumns + j;

                if (index >= skinCount) break;

                Skin* skin = FindSkin(cat->metaCache.metadata[index].id);
                Gui_SkinSlot(skin, cellH);
            }

            Gui_SameLine(skinColumns, NULL);

            static char viewBtnName[16] = {"View##"};

            for (int j = 0; j < skinColumns; j++) {
                int index = i * skinColumns + j;

                if (index >= skinCount) break;

                int skinid = CatalogCtx.pagesCache[CatalogCtx.tab].metaCache.metadata[index].id;

                snprintf(viewBtnName + 6, sizeof(viewBtnName) - 6, "%i", skinid);
                if (Gui_Button(viewBtnName)) {

                    DownloadSkins_InitView(
                        &cat->metaCache.metadata[index],
                        FindSkin(cat->metaCache.metadata[index].id)
                    );
                }
            }
        }

        content.height = BUTTON_HEIGHT * skinRows + cellH * skinRows + ELEMENT_PADDING * (skinRows * 2 + 1);

        Gui_SameLine(1, NULL);

        if (CatalogCtx.state) {
            Gui_Label(CatalogCtx.state);

            Rectangle last = Gui_GetLastBounds();
            content.height += last.height;
        }

        bool canLoadNewSkins = -scroll.y + view.height > content.height && cat->state == PAGE_STATE_DONE;

        if (canLoadNewSkins) {
            Http_GetSkins(cat->current, tab, searchKey, SkinListDownloaded);

            cat->state = PAGE_STATE_LOADING;
            cat->current++;

            CatalogCtx.state = "Loading skins...";
        }
    }
    Gui_EndScrollPanel();
}

// SKIN BROWSER

void DownloadSkins_Init(void) {
    for (int i = 0; i < SKINS_TYPE_COUNT; i++) {
        CatalogCtx.pagesCache[i].metaCache.count = 0;
        CatalogCtx.pagesCache[i].current = 1;
        CatalogCtx.pagesCache[i].state = (i == SKINS_TYPE_SEARCH)? PAGE_STATE_END : PAGE_STATE_DONE;
    }

    CatalogCtx.skinCache.count = 0;
}

void DownloadSkins_Draw(void) {
    Gui_BeginWindow("BROWSE SKINS");

    Gui_SetPositionMode(FROM_BOTTOM);
    if(Gui_Button("Back") || Gui_IsNavBack()) {
        if (ViewCtx.meta == NULL) {
            for (int i = 0; i < CatalogCtx.skinCache.count; i++) UnloadTexture(CatalogCtx.skinCache.skins[i].texture);
            Gui_ChangeScreen(&SelectSkin);
            return;
        } else {
            DownloadSkins_DeinitView();
        }
    }

    if (ViewCtx.meta == NULL) {
        DownloadSkin_CatalogDraw();
    } else {
        DownloadSkins_DrawView();
    }

    Gui_EndWindow();
}

Screen DownloadSkins = {
    DownloadSkins_Init,
    DownloadSkins_Draw
};