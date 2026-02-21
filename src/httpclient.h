#ifndef HTTP_H
#define HTTP_H
#include <stdbool.h>

typedef struct {
    int error;
    char* data;
    int length;
} HttpResponse;

typedef void (*HttpResponseCallback)(HttpResponse* response, void* userdata);

// General
void Http_Init(void);
void Http_Update(void);

//Methods
void Http_Get(const char* url, HttpResponseCallback callback, void* userdata, int bufsize);

// Misc
void Http_URLDecode(char* s);

// Skins
typedef enum {
    SKINS_TYPE_FEATURED, 
    SKINS_TYPE_NEW, 
    SKINS_TYPE_BEST,
    SKINS_TYPE_SEARCH,
    
    SKINS_TYPE_COUNT
} SkinsType;

typedef enum {
    PATH_CACHE,
    PATH_SKINS
} SkinDownloadPath;

typedef struct {
    int id;
    char name[64];
    char author[64];
    int date;
} SkinMetadata;

typedef struct {
    int page;
    SkinsType type;
    SkinMetadata skins[5]; 
} SkinList;

typedef void(*SkinListCallback)(int error, SkinList* response);
typedef void(*SkinDownloadCallback)(int error, int id);

void Http_GetSkins(int page, SkinsType type, char* search, SkinListCallback responseCallback);

void Http_DownloadSkin(int id, SkinDownloadPath path, SkinDownloadCallback callback);

const char* Http_Error(int err);
#endif // HTTP_H