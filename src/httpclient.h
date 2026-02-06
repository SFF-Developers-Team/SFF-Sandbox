#ifndef HTTP_H
#define HTTP_H
#include <stdbool.h>

typedef enum {
    HTTP_NO_ERROR,
    HTTP_NOT_FOUND,
    HTTP_NO_BUFFER,
    HTTP_BUFFER_TOO_SMALL,
    HTTP_UNEXPECTED_EOF,
    HTTP_INVALID_HTTP_STATUS,
    HTTP_CANCELLED,
    HTTP_OTHER
} HttpError;

typedef struct {
    HttpError error;
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
    SKINS_TYPE_SEARCH
} SkinsType;

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

typedef void(*SkinListCallback)(HttpError error, SkinList* response);
typedef void(*SkinDownloadCallback)(HttpError error, int id);

void Http_GetSkins(int page, SkinsType type, char* search, SkinListCallback responseCallback);

void Http_DownloadSkin(int id, const char* path, SkinDownloadCallback callback);
#endif // HTTP_H