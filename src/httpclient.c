#include "httpclient.h"
#include "win_minimal.h"
#include <curl/curl.h>
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static CURLM* g_multi;

// General
void Http_Init(void) {
    curl_global_init(CURL_GLOBAL_ALL);
    g_multi = curl_multi_init();
}

typedef struct HttpRequestCtx {
    HttpResponseCallback callback;
    void* userdata;

    CURL* handle;
    char* buffer;
    int size;
    int capacity;
} HttpRequestCtx;

void Http_Update(void) {
    int running = 0;
    curl_multi_perform(g_multi, &running);

    int msgs;
    CURLMsg* msg;

    while ((msg = curl_multi_info_read(g_multi, &msgs))) {
        if (msg->msg == CURLMSG_DONE) {
            CURL* easy = msg->easy_handle;

            HttpRequestCtx* ctx;
            curl_easy_getinfo(easy, CURLINFO_PRIVATE, &ctx);

            HttpResponse resp = {0};
            resp.data = ctx->buffer;
            resp.length = ctx->size;
            resp.error = (msg->data.result == CURLE_OK)
                ? 0
                : msg->data.result;

            ctx->callback(&resp, ctx->userdata);

            curl_multi_remove_handle(g_multi, easy);
            curl_easy_cleanup(easy);
            MemFree(ctx->buffer);
            MemFree(ctx);
        }
    }
}

// Methods
static size_t Curl_Write(void* ptr, size_t size, size_t nmemb, void* userdata) {
    HttpRequestCtx* ctx = userdata;
    int total = (int)(size * nmemb);

    if (ctx->size + total > ctx->capacity) {
        return 0;
    }

    memcpy(ctx->buffer + ctx->size, ptr, total);
    ctx->size += total;

    return total;
}

void Http_Get(const char* url, HttpResponseCallback callback, void* userdata, int bufsize) {
    // oh no small allocation for callback and userdata :sob:
    HttpRequestCtx* ctx = MemAlloc(sizeof(HttpRequestCtx));
    ctx->callback = callback;
    ctx->userdata = userdata;
    ctx->handle = curl_easy_init();

    if (!ctx->handle) {
        MemFree(ctx);
        return; // TODO: error
    }

    ctx->buffer = MemAlloc(bufsize);
    ctx->capacity = bufsize;
    ctx->size = 0;

    curl_easy_setopt(ctx->handle, CURLOPT_URL, url);
    curl_easy_setopt(ctx->handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(ctx->handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(ctx->handle, CURLOPT_WRITEFUNCTION, Curl_Write);
    curl_easy_setopt(ctx->handle, CURLOPT_WRITEDATA, ctx);
    curl_easy_setopt(ctx->handle, CURLOPT_USERAGENT, "sandbox-for-friends/1.0");

    curl_easy_setopt(ctx->handle, CURLOPT_PRIVATE, ctx);

    curl_multi_add_handle(g_multi, ctx->handle);
}

// Misc
void Http_URLDecode(char *s) {
    char *o = s;

    for (; *s; s++, o++) {
        if (*s == '+') {
            *o = ' ';
        } else if (*s == '%' && s[1] && s[2]) {
            char hex[3] = { s[1], s[2], 0 };
            *o = (char) strtol(hex, NULL, 16);
            s += 2;
        } else {
            *o = *s;
        }
    }
    
    *o = 0;
}

// Skins
void Http_GetSkinsResp(HttpResponse* resp, void* userdata) {
    if (resp->error != HTTP_NO_ERROR) goto done;

    SkinList list = {0};

    int amountSplited;
    char** splited = TextSplit(resp->data, '&', &amountSplited);

    if (strcmp(*splited, "s=") != 0) {
        sscanf(*splited, "s=%d/", &list.page);
        *splited += (int)(strchr(*splited, '/') - *splited + 1);

        if (strcmp(*splited, "featured") == 0) {
            list.type = SKINS_TYPE_FEATURED;
        } else if (strcmp(*splited, "new") == 0) {
            list.type = SKINS_TYPE_NEW;
        } else if (strcmp(*splited, "best") == 0) {
            list.type = SKINS_TYPE_BEST;
        } else if (strcmp(*splited, "search") == 0) {
            list.type = SKINS_TYPE_SEARCH;
        }
    }

    splited++;
    amountSplited--;

    while (amountSplited--) {
        int i;

        switch (**splited) {
            case 'i':
                int id;
                sscanf(*splited, "id%d=%d", &i, &id);

                list.skins[i-1].id = id;
                break;

            case 'n':
                sscanf(*splited, "n%d=", &i);
                *splited += 3;

                Http_URLDecode(*splited);

                strncpy(list.skins[i-1].name, *splited, sizeof(list.skins[i].name));
                break;
            
            case 'a':
                sscanf(*splited, "a%d=", &i);
                *splited += 3;

                Http_URLDecode(*splited);

                strncpy(list.skins[i-1].author, *splited, sizeof(list.skins[i].author));
                break;

            case 'd':
                int date;
                sscanf(*splited, "date%d=%d", &i, &date);

                list.skins[i-1].date = date;
                break;
        }

        splited++;
    }

done:
    SkinListCallback callback = (SkinListCallback)userdata;
    callback(resp->error, &list);
}

void Http_GetSkins(int page, SkinsType type, char* search, SkinListCallback responseCallback) {
    static char url[128];
    static char* types[] = {"featured", "new", "best", "search"};

    snprintf(url, sizeof(url), "http://mineblocks.com/1/scripts/getSkins?page=%d&type=%s", page, types[type]);

    if (type == SKINS_TYPE_SEARCH && search != NULL) {
        int pos = strlen(url);
        snprintf(url + pos, sizeof(url) - pos, "&key=%s", search);
    }

    Http_Get(url, Http_GetSkinsResp, (void*)responseCallback, 1024);
}

typedef struct {
    SkinDownloadCallback callback;
    int id;
    const char* path;
} SkinDownloadCtx;

void Http_DownloadSkinResp(HttpResponse* resp, void* userdata) {
    if (resp->error != HTTP_NO_ERROR) goto done;
    SkinDownloadCtx* ctx = (SkinDownloadCtx*)userdata;

    SaveFileData(ctx->path, resp->data, resp->length);

done:
    SkinDownloadCallback callback = ctx->callback;
    int id = ctx->id;
    MemFree(ctx->path);
    MemFree(ctx);

    callback(resp->error, id);
}

void Http_DownloadSkin(int id, const char* path, SkinDownloadCallback callback) {
    static char url[64];

    snprintf(url, sizeof(url), "http://mineblocks.com/1/skins/%d.png", id);

    SkinDownloadCtx* ctx = MemAlloc(sizeof(SkinDownloadCtx));
    ctx->callback = callback;
    ctx->id = id;
    ctx->path = MemAlloc(TextLength(path));
    TextCopy(ctx->path, path);

    Http_Get(url, Http_DownloadSkinResp, ctx, 8192);
}