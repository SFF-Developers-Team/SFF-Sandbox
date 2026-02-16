#define RAYGUI_IMPLEMENTATION
#define RAYGUI_PANEL_BORDER_WIDTH 2
#include "screens.h"
#include "raygui.h"
#include "utils.h"
#include "game.h"
#include <stdlib.h>
#include <stdint.h>

#define MAX_NAV_ITEMS 256
#define DEFAULT_PROPS_COUNT (RAYGUI_MAX_PROPS_BASE + RAYGUI_MAX_PROPS_EXTENDED)
#define TEXT_WEIGHT (DEFAULT_PROPS_COUNT + 1)

Screen* currentScreen;

struct CustomStyleProp {
    int prop;
    Color color;
};

struct CustomStyleProp CustomGuiStyle[] = {
    {BORDER_COLOR_NORMAL, {79, 108, 212, 255}},
    {BASE_COLOR_NORMAL, {41, 41, 91, 255}},
    {TEXT_COLOR_NORMAL, {255, 255, 255, 255}},
    
    {BORDER_COLOR_FOCUSED, {154, 157, 255, 255}},
    {BASE_COLOR_FOCUSED, {91, 95, 237, 255}},
    {TEXT_COLOR_FOCUSED, {255, 255, 255, 255}},

    {BORDER_COLOR_PRESSED, {0, 0, 134, 255}},
    {BASE_COLOR_PRESSED, {0, 127, 255, 255}},
    {TEXT_COLOR_PRESSED, {255, 255, 255, 255}},

    {BORDER_COLOR_DISABLED, {16, 10, 124, 255}},
    {BASE_COLOR_DISABLED, {0, 0, 93, 255}},
    {TEXT_COLOR_DISABLED, {128, 128, 128, 255}},
    
    {BACKGROUND_COLOR, {0, 0, 73, 255}},
    {LINE_COLOR, {50, 92, 203, 255}},
};

Font customGuiFont;
Font customGuiBoldFont;

#define SET_DEFAULT_FONT() GuiSetFont(customGuiFont);
#define SET_BOLD_FONT() GuiSetFont(customGuiBoldFont);
#define Gui_GetTextSize() GuiGetStyle(DEFAULT, TEXT_SIZE)
#define Gui_SetTextSize(size) GuiSetStyle(DEFAULT, TEXT_SIZE, size)
#define Gui_CurrentSelected() Context.selectedId == Context.lastId

int Gui_CountListElements(const char* list) { 
    if (!strlen(list)) return 0;
    
    int count = 1;
    for (int i = 0; i < strlen(list); i++) if (list[i] == ';') count++;

    return count;
}

struct GuiContext {
    unsigned int lastId;
    unsigned int selectedId;
    int activeInput;
    
    float x, startY, endY, w;

    bool fromStart;
    bool labelNext;
    bool controlLocked;
    bool scrollPanel;
    bool inScrollPanel;
    bool wasScrollPanel;

    int sameLine;
    int cols;
    float* ratios;

    Rectangle lastBounds;
    TextWeight textWeight;

    struct GuiNavItem {
        unsigned int id;
        Rectangle rect;
    } navItems[MAX_NAV_ITEMS];
    int navCount;
} Context;


void Gui_SetStyle(int control, int prop, int val) {
    if (prop == TEXT_WEIGHT) {
        Context.textWeight = val;

        if (val == TEXT_DEFAULT) {
            SET_DEFAULT_FONT();
        } else if (val == TEXT_BOLD) {
            SET_BOLD_FONT();
        }

        return;
    }

    GuiSetStyle(control, prop, val);
}

int Gui_GetStyle(int control, int prop) {
    if (prop == TEXT_WEIGHT) {
        return Context.textWeight;
    }

    return GuiGetStyle(control, prop);
}

void Gui_Init(void) {
    for (int i = 0; i < sizeof(CustomGuiStyle) / sizeof(struct CustomStyleProp); i++) {
        GuiSetStyle(DEFAULT, CustomGuiStyle[i].prop, ColorToInt(CustomGuiStyle[i].color));
    }

    customGuiFont = LoadFont("resources/fonts/font.fnt");
    customGuiBoldFont = LoadFont("resources/fonts/boldfont.fnt");

    Gui_SetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    Gui_SetStyle(LABEL, TEXT_WEIGHT, TEXT_DEFAULT);
    Gui_SetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    Gui_SetTextSize(MENU_TEXT_SIZE);

    currentScreen = &MainMenu;
}

// Screen

void Gui_ChangeScreen(Screen* screen) {
    if (screen->Init != NULL) screen->Init();
    currentScreen = screen;
}

// Internal

Rectangle Gui_CalculateBounds(float height) {
    float w = (Context.w / Context.sameLine) - (ELEMENT_PADDING * (Context.sameLine-1) / 2);
    float x = Context.x;

    if (Context.ratios != NULL) {
        for (int i = 0; i < Context.cols; i++) x += Context.w*Context.ratios[i];
        w = (Context.w - ELEMENT_PADDING / 2) * Context.ratios[Context.cols];
    } else {
        x += (w*Context.cols) + ELEMENT_PADDING * Context.cols;
    }

    Context.lastBounds = (Rectangle){x, (Context.fromStart)? Context.startY : Context.endY - height, w, height};

    return Context.lastBounds;
}

void Gui_Next() {
    if (GuiGetState() != STATE_DISABLED) GuiSetState(STATE_NORMAL); // cuz previous may be element focused

    Context.cols++;

    if (Context.cols >= Context.sameLine) {
        Context.sameLine = 1;
        Context.cols = 0;
        Context.ratios = NULL;
    } else goto end;

    float add = Context.lastBounds.height + ELEMENT_PADDING;

    if (Context.fromStart) {
        Context.startY += add;
    } else {
        Context.endY -= add;

        if (Context.labelNext) Context.endY -= Gui_GetTextSize();
    }

end:
    Context.labelNext = false;
}

unsigned int Gui_CalculateHash(const char* name) {
    int hash = 2166136261u; // FNV-1a

    while (*name) {
        hash ^= (unsigned char)*name++;
        hash *= 16777619u;
    }

    return hash;
}

const char* Gui_RemoveHash(const char* label) {
    const char* sep = strstr(label, "##");
    return sep ? TextSubtext(label, 0, sep - label) : label;
}

bool Gui_IsNavRight() {
    return IsGamepadButtonPressed(mainGamepad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || IsKeyPressed(KEY_RIGHT);
}

bool Gui_IsNavLeft() {
    return IsGamepadButtonPressed(mainGamepad, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || IsKeyPressed(KEY_LEFT);
}

bool Gui_IsNavUp() {
    return IsGamepadButtonPressed(mainGamepad, GAMEPAD_BUTTON_LEFT_FACE_UP) || IsKeyPressed(KEY_UP);
}

bool Gui_IsNavDown() {
    return IsGamepadButtonPressed(mainGamepad, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || IsKeyPressed(KEY_DOWN);
}

bool Gui_IsNavEnter() {
    return IsGamepadButtonPressed(mainGamepad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) || IsKeyPressed(KEY_ENTER);
}

bool Gui_IsNavBack() {
    return IsGamepadButtonPressed(mainGamepad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) || IsKeyPressed(KEY_ESCAPE);
}

bool Gui_PrevScreenCalled() {
    return !GuiIsLocked() && Gui_IsNavBack();
}

/**
 * @name Should be unique!! Use ## for unique names Example: "View skin##267316"
 * @return Is element pressed
 */
bool Gui_RegisterElement(const char* name, Rectangle bounds) {
    bool pressed = false;

    if (name == NULL || (Context.scrollPanel && !Context.inScrollPanel) || (!Context.scrollPanel && Context.inScrollPanel)) {
        return false;
    }

    if (GuiGetState() != STATE_DISABLED && Context.navCount < MAX_NAV_ITEMS) {
        unsigned int id = Gui_CalculateHash(name);

        if (Context.selectedId == 0) Context.selectedId = id;

        Context.lastId = id;

        Context.navItems[Context.navCount++] = (struct GuiNavItem){id, bounds};

        if (Context.selectedId == id) {
            GuiSetState(STATE_FOCUSED);

            if (Gui_IsNavEnter() && !Context.controlLocked) {
                GuiSetState(STATE_PRESSED);
                pressed = true;
            }
        }
    }

    return pressed;
}

struct GuiNavItem* Gui_FindItem(unsigned int id) {
    for (int i = 0; i < Context.navCount; i++) {
        if (Context.navItems[i].id == id) return &Context.navItems[i];
    }

    return NULL;
}

void Gui_MoveFocus(struct GuiNavItem* from, int dirX, int dirY) {
    int best_id = -1;
    float best_score = -100000000.f;
    
    for (int i = 0; i < Context.navCount; i++) {
        struct GuiNavItem* candidate = &Context.navItems[i];
        
        if (candidate->id == from->id) continue;
        
        float dx = (candidate->rect.x + candidate->rect.width / 2) - 
                   (from->rect.x + from->rect.width / 2);
        float dy = (candidate->rect.y + candidate->rect.height / 2) - 
                   (from->rect.y + from->rect.height / 2);
        
        if (dirX != 0) {
            if (dirX > 0 && dx <= 0) continue;
            if (dirX < 0 && dx >= 0) continue;
            
            float y_diff = fabsf(dy);
            float x_diff = fabsf(dx);
            
            if (y_diff > from->rect.height * 1.5f) continue;
            
            float alignment_score = 1.0f / (y_diff + 1.0f);
            float distance_score = 1.0f / (x_diff + 1.0f);
            float score = alignment_score * 0.7f + distance_score * 0.3f;
            
            if (score > best_score) {
                best_score = score;
                best_id = candidate->id;
            }
        } else if (dirY != 0) {
            if (dirY > 0 && dy <= 0) continue;
            if (dirY < 0 && dy >= 0) continue;
            
            float x_diff = fabsf(dx);
            float y_diff = fabsf(dy);
            
            if (x_diff > from->rect.width * 1.5f) continue;
            
            float alignment_score = 1.0f / (x_diff + 1.0f);
            float distance_score = 1.0f / (y_diff + 1.0f);
            float score = alignment_score * 0.7f + distance_score * 0.3f;
            
            if (score > best_score) {
                best_score = score;
                best_id = candidate->id;
            }
        }
    }
    
    if (best_id != -1) {
        Context.selectedId = best_id;
    }
}


// Manipulation

void Gui_BeginWindow(const char* name) {
    int scrWidth = GetScreenWidth();
    int scrHeight = GetScreenHeight();

    // nums
    Context.lastId = 0;
    Context.sameLine = 1;
    Context.cols = 0;
    Context.ratios = NULL;

    Vector2 windowSize = {MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT};
    
    // flags
    Context.fromStart = true;
    Context.labelNext = false;
    Context.controlLocked = false;
    Context.scrollPanel = false;
    Context.wasScrollPanel = false;
    
    Context.navCount = 0;

    // position context
    Context.x = (scrWidth - windowSize.x) / 2 + ELEMENT_PADDING * 2;
    Context.startY = (scrHeight - windowSize.y) / 2;
    Context.endY = Context.startY + MENU_WINDOW_HEIGHT - ELEMENT_PADDING;
    Context.w = windowSize.x - ELEMENT_PADDING * 2;

    Rectangle panelBounds = (Rectangle){Context.x, Context.startY, windowSize.x, windowSize.y}; 

    GuiPanel(panelBounds, NULL);
    Context.x += ELEMENT_PADDING;
    Context.startY += ELEMENT_PADDING;

    Gui_SetStyle(LABEL, TEXT_WEIGHT, TEXT_BOLD);
    Gui_StyleBackup(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    Gui_StyleBackup(LABEL, TEXT_SIZE, MENU_TEXT_SIZE_BIG);
    
    Gui_Label(name);

    Gui_SetStyle(LABEL, TEXT_WEIGHT, TEXT_DEFAULT);
    Gui_StyleRestore(LABEL, TEXT_ALIGNMENT);
    Gui_StyleRestore(LABEL, TEXT_SIZE);
    
    Gui_Next();
}

void Gui_EndWindow(void) {
    struct GuiNavItem* current = Gui_FindItem(Context.selectedId);

    if (!current) { Context.selectedId = 0; return; }

    if (GuiIsLocked() || Context.navCount == 0) return;

    if (Gui_IsNavUp()) {
        Gui_MoveFocus(current, 0, -1);
    }
    else if (Gui_IsNavDown()) {
        Gui_MoveFocus(current, 0, 1);
    }
    else if (Gui_IsNavLeft()) {
        Gui_MoveFocus(current, -1, 0);
    }
    else if (Gui_IsNavRight()) {
        Gui_MoveFocus(current, 1, 0);
    }

    if (!Context.wasScrollPanel && Context.inScrollPanel) {
        Context.inScrollPanel = false;
    }
}

void Gui_SetPositionMode(PositionMode mode) {
    Context.fromStart = mode == FROM_TOP;
}

float Gui_GetRemainingSpace(void) {
    float ret = Context.endY - Context.startY;
    
    if (Context.labelNext) {
        ret -= Gui_GetTextSize();
    }

    return ret;
}

Rectangle Gui_GetLastBounds(void) {
    return Context.lastBounds;
}

void Gui_SameLine(int elements, float* ratios) {
    Context.sameLine = elements;
    Context.ratios = ratios;
}

void Gui_Disable(void) {
    GuiDisable();
}

void Gui_Enable(void) {
    GuiEnable();
}

void Gui_LabelNextElement(const char* label) {
    if (!Context.fromStart) {
        Context.endY -= Gui_GetTextSize();
    }

    Gui_StyleBackup(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    GuiLabel(Gui_CalculateBounds(Gui_GetTextSize()), label);
    Gui_StyleRestore(LABEL, TEXT_ALIGNMENT);

    if (Context.fromStart) {
        Context.startY += Gui_GetTextSize();
    } else {
        Context.endY += Gui_GetTextSize();
    }
    
    Context.labelNext = true;
}

// Widgets

void Gui_Label(const char* label) {
    Font currentFont = GuiGetFont();

    GuiLabel(Gui_CalculateBounds(currentFont.baseSize), label);
    Gui_Next();
}

bool Gui_RawButton(const char* label, Rectangle bounds) {
    bool pressed = Gui_RegisterElement(label, bounds);
    bool ret = GuiButton(bounds, Gui_RemoveHash(label)) || pressed;

    return ret;
}

bool Gui_Button(const char* label) {
    Rectangle b = Gui_CalculateBounds(BUTTON_HEIGHT);

    bool ret = Gui_RawButton(label, b);
    Gui_Next();

    return ret;
}

int Gui_ListView(float height, const char* list, int* scrollIndex, int* selected) {
    Rectangle b = Gui_CalculateBounds(height);

    bool pressed = Gui_RegisterElement(list, b);

    const char **items = NULL;
    int itemCount = 0;
    static int focus = -1;

    if (list != NULL) items = GuiTextSplit(list, ';', &itemCount, NULL);

    if (!GuiIsLocked()) focus = -1;

    if (Gui_CurrentSelected() && pressed && !GuiIsLocked()) {
        GuiLock();
    }

    if (Gui_CurrentSelected() && GuiIsLocked()) {
        if (Gui_IsNavUp() && focus > 0) focus--;
        if (Gui_IsNavDown()) focus++;
        if (Gui_IsNavBack()) GuiUnlock(); 
        if (Gui_IsNavEnter() && focus >= 0) { *selected = focus; GuiUnlock(); }

        if (focus < 0) focus = 0;
        if (focus >= Gui_CountListElements(list)) focus = 0;
    }

    int ret = GuiListViewEx(b, items, itemCount, scrollIndex, selected, &focus);
    
    Gui_Next();

    return ret;
}

bool Gui_TextInput(const char* label, char* out, int outSize) {
    Rectangle b = Gui_CalculateBounds(INPUT_HEIGHT);
    bool ret = false;

    Gui_RegisterElement(label, b);
    
    if (GuiTextBox(b, out, outSize, Context.selectedId == Context.lastId)) {
        Context.selectedId = Context.lastId;
        ret = true;
    }

    Gui_Next();

    return ret;
}

int Gui_ComboBox(const char* list, int* selected) {
    Rectangle b = Gui_CalculateBounds(INPUT_HEIGHT);

    Gui_RegisterElement(list, b);

    if (GuiGetState() == STATE_PRESSED) {
        int count = Gui_CountListElements(list);

        *selected += 1;
        if (*selected >= count) *selected = 0;
    }

    int ret = GuiComboBox(b, Gui_RemoveHash(list), selected);

    Gui_Next();

    return ret;
}

void Gui_Texture(Texture2D* tex, Rectangle* src, float height) {
    if (height == 0.0f) height = (float)tex->height;

    // texW w
    // texH h

    Rectangle source = (src != NULL)? *src : (Rectangle){0, 0, (float)tex->width, (float)tex->height};
    Rectangle dest = Gui_CalculateBounds(0.f);

    float width = ((tex->width * height) / tex->height);

    if (width > dest.width) {
        width = dest.width;
        height = (width * tex->height) / tex->width;
    }

    dest = Gui_CalculateBounds(height);
    DrawTexturePro(*tex, source, dest, (Vector2){0.f}, 0.0f, WHITE);
    Gui_Next();
}

int Gui_CustomElement(float height, const char* label, DrawElementCallback drawCallback, void* userdata, int flags) {
    Rectangle b = Gui_CalculateBounds(height);
    int ret = 0;

    if (flags & FLAG_CUSTOM_REGISTER) {
        Gui_RegisterElement(label, b);
    }

    if (flags & FLAG_CUSTOM_DRAW_BG) {
        GuiDrawRectangle(b, 1, 
            GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_NORMAL)), 
            GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_NORMAL))
        );
    }

    ret = drawCallback(b, (label != NULL)? Gui_RemoveHash(label) : NULL, userdata);

    Gui_Next();

    return ret;
}

void Gui_RawSkinSlot(Skin* skin, Rectangle bounds) {
    int skinH = bounds.height - ELEMENT_PADDING * 2;
    int skinW = SKIN_FRAME_WIDTH * (skinH / SKIN_FRAME_HEIGHT);

    Skin_Draw(skin, (Rectangle){bounds.x + (bounds.width - skinW) / 2, bounds.y + ELEMENT_PADDING, skinW, skinH});
}

void Gui_SkinSlot(Skin* skin, float height) {
    Rectangle rect = Gui_CalculateBounds(height);

    GuiDrawRectangle(rect, 1, 
        GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_NORMAL)), 
        GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_NORMAL))
    );

    Gui_RawSkinSlot(skin, rect);
    Gui_Next();
}

static float x, sy, ey, w;

void Gui_BeginScrollPanel(const char* label, float height, Rectangle content, Vector2* scroll, Rectangle* view) {
    if (Context.scrollPanel) TraceLog(LOG_FATAL, "Rendering scroll panel inside a scroll panel is not allowed.");
    
    Rectangle b = Gui_CalculateBounds(height);

    bool pressed = Gui_RegisterElement(label, b);

    if (Gui_CurrentSelected() && pressed) {
        Context.inScrollPanel = true;
        // GuiLock();
    }

    if (Context.inScrollPanel && Gui_IsNavBack()) {
        Context.inScrollPanel = false;
        // GuiUnlock(); 
    }

    struct GuiNavItem* current = Gui_FindItem(Context.selectedId);

    GuiScrollPanel(b, NULL, content, scroll, view);

    // TODO: auto scroll on gamepad

    view->width = b.width - ELEMENT_PADDING * 2;

    BeginScissorMode(view->x, view->y, view->width, view->height);

    Context.scrollPanel = true;

    // Save context
    x = Context.x;
    sy = Context.startY;
    ey = Context.endY;
    w = Context.w;

    // Inner context
    Context.x = view->x + ELEMENT_PADDING + scroll->x;
    Context.startY = view->y + ELEMENT_PADDING + scroll->y;
    Context.endY = view->y + view->height - ELEMENT_PADDING + scroll->y;
    Context.w = view->width;
}

void Gui_EndScrollPanel() {
    Context.scrollPanel = false;
    Context.wasScrollPanel = true;

    // Restore context
    Context.x = x;
    Context.startY = sy;
    Context.endY = ey;
    Context.w = w;
    
    EndScissorMode();
    Gui_Next();
}

void Gui_Tabs(const char* list, int* selected) {
    Rectangle b = Gui_CalculateBounds(INPUT_HEIGHT);

    Gui_RegisterElement(list, b);
    int count = Gui_CountListElements(list);

    b.width /= count;
    b.width -= GuiGetStyle(TOGGLE, GROUP_PADDING);

    GuiToggleGroup(b, list, selected);

    Gui_Next();
}

// Auto layout

int Gui_CenterMenu(const char* names, Vector2 buttonSize) {
    int count = 0;
    int ret = -1;

    const char** namesSplited = GuiTextSplit(names, ';', &count, NULL);

    float height = count * buttonSize.y + count * ELEMENT_PADDING;

    int scrWidth = GetScreenWidth();
    int scrHeight = GetScreenHeight();

    static int selected = -1;
    static bool dpad = false;

    if (Gui_IsNavUp()) {
        dpad = true;

        if (selected > 0) selected--;
    }

    if (Gui_IsNavDown()) {
        dpad = true;

        if (selected < count-1) selected++;
    }

    Vector2 mdelta = GetMouseDelta();
    if (mdelta.x != 0.0f || mdelta.y != 0.0f) {
        dpad = false;
    }

    for (int i = 0; i < count; i++) {
        Rectangle bounds = {
            (scrWidth - buttonSize.x) / 2,
            (scrHeight - height) / 2 + (buttonSize.y + ELEMENT_PADDING) * i,
            buttonSize.x, buttonSize.y
        };

        if (i == selected && dpad) {
            GuiSetState(STATE_FOCUSED);

            if (Gui_IsNavEnter()) {
                GuiSetState(STATE_PRESSED);
                ret = i;
            }
        }

        if (GuiButton(bounds, namesSplited[i])) ret = i;

        GuiSetState(STATE_NORMAL);
    }

    return ret;
}