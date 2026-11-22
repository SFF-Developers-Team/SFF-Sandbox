#ifndef GUI_H
#define GUI_H

#include "defines.h"
#include "raylib.h"
#include "skin.h"
#include <stdbool.h>

#define BUTTON_HEIGHT 30
#define INPUT_HEIGHT 30
#define ELEMENT_PADDING 8

#define MENU_TEXT_SIZE 20
#define MENU_TEXT_SIZE_BIG 20

#define MENU_WINDOW_WIDTH 400
#define MENU_WINDOW_HEIGHT 500

#define Gui_StyleBackup(control, prop, new) \
    int backup_##control##prop = Gui_GetStyle(control, prop); \
    Gui_SetStyle(control, prop, new)

#define Gui_StyleRestore(control, prop) \
    Gui_SetStyle(control, prop, backup_##control##prop)


#define FLAG_CUSTOM_DRAW_BG     (1 << 0)
#define FLAG_CUSTOM_REGISTER    (1 << 1)

typedef struct Screen {
    void(*Init)(void);
    void(*Draw)(void);
} Screen;

extern Screen* currentScreen;

extern Screen MainMenu;
extern Screen SelectWorld;
extern Screen CreateWorld;
extern Screen SelectSkin;
extern Screen DownloadSkins;
extern Screen GameplayScreen;

extern bool shouldGenerateWorld;
extern char currentWorldName[MAX_WORLD_NAME];

extern const char* error;

typedef enum TextWeight {
    TEXT_DEFAULT,
    TEXT_BOLD
} TextWeight;

typedef enum PositionMode {FROM_TOP = 1, FROM_BOTTOM = 0} PositionMode;

typedef int(*DrawElementCallback)(Rectangle bounds, const char* label, void* userdata);
typedef void(*DrawScrollPanelContent)(Rectangle view);

void Gui_Init(void);

// Screens

void Gui_ChangeScreen(Screen* screen);
bool Gui_PrevScreenCalled();

// Window

void Gui_BeginWindow(const char* name);
void Gui_EndWindow(void);

// Manipulation

void Gui_SetPositionMode(PositionMode mode);
void Gui_SetStyle(int control, int prop, int val);
int Gui_GetStyle(int control, int prop);

// Custom
bool Gui_IsNavRight();
bool Gui_IsNavLeft();
bool Gui_IsNavUp();
bool Gui_IsNavDown();
bool Gui_IsNavEnter();
bool Gui_IsNavBack();

/**
 * @name Should be unique!! Use ## for unique names Example: "View skin##267316"
 * @return Is element pressed
 */
bool Gui_RegisterElement(const char* name, Rectangle bounds);

// Get free height between last top y and last bottom y
float Gui_GetRemainingSpace(void);
Rectangle Gui_GetLastBounds(void);

void Gui_SameLine(int elements, float* ratios);
void Gui_LabelNextElement(const char* label);

/**
 * @brief Make next elements unclickable
 */
void Gui_Disable();

/**
 * @brief Make next elements clickable
 */
void Gui_Enable();

// Widgets

void Gui_Label(const char* label);
bool Gui_Button(const char* label);
int Gui_ListView(float height, const char* list, int* scrollIndex, int* selected);
bool Gui_TextInput(const char* label, char* out, int outSize);
int Gui_ComboBox(const char* list, int* selected);
void Gui_Texture(Texture2D* tex, Rectangle* src, float height);
int Gui_CustomElement(float height, const char* label, DrawElementCallback drawCallback, void* userdata, int flags);

void Gui_BeginScrollPanel(const char* label, float height, Rectangle content, Vector2* scroll, Rectangle* view);
void Gui_EndScrollPanel();

void Gui_Tabs(const char* list, int* selected);

void Gui_SkinSlot(Skin* skin, float height);

// Raw widgets
bool Gui_RawButton(const char* label, Rectangle bounds);
void Gui_RawSkinSlot(Skin* skin, Rectangle bounds);

// WARNING: This function dont need context (you may not call Gui_WindowBegin() and Gui_WindowEnd() as well) and this menu WON'T render in window
int Gui_CenterMenu(const char* names, Vector2 buttonSize);
#endif