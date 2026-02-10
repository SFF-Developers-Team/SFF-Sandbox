#include "files.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"

#define MAKE_DIR(path) if (MakeDirectory(path) != 0) TraceLog(LOG_ERROR, "SAVES: Failed to create directory: %s", path)

char* GetDataDirectory(void) {
    static char path[MAX_PATH] = { 0 };

#ifdef _WIN32
    const char* appdata = getenv("AppData");

    if (appdata == NULL) {
        TraceLog(LOG_FATAL, "AppData PATH variable is not set!");
    }

    strcpy(path, appdata);
    strcat(path, "\\sandboxforfriends\\");
#else 
    const char* xdg = getenv("XDG_DATA_HOME");

    if (xdg) {
        strcpy(path, xdg);
        strcat(path, "/sandboxforfriends/");
    } else {
        const char* home = getenv("HOME");
        strcpy(path, home);
        strcat(path, "/.local/share/sandboxforfriends/");
    }
#endif
    
    TraceLog(LOG_INFO, "SAVES: Working directory: %s", path);

    return path;
}

char* GetDataSubdirectory(const char* subdir) {
    char* home = GetDataDirectory();
    strcat(home, subdir);

    return home;
}

void InitFiles(void) {
    // home://
    char* dir = GetDataDirectory();
    MAKE_DIR(dir);

    // home://saves
    dir = GetDataSubdirectory("saves");
    MAKE_DIR(dir);

    // home://skins
    dir = GetDataSubdirectory("skins");
    MAKE_DIR(dir);

    // home://cache
    dir = GetDataSubdirectory("cache");
    MAKE_DIR(dir);
}

const char* JoinFilePathListFilenames(FilePathList list) {
    if (list.count == 0) return NULL;

    int totalLength = 0;
    for (int i = 0; i < list.count; i++) {
        totalLength += TextLength(GetFileNameWithoutExt(list.paths[i]));
        if (i + 1 < list.count) totalLength += 1; // ;
    }

    char* result = (char*)MemAlloc(totalLength + 1);
    if (!result) return NULL;

    result[0] = '\0'; // cuz allocated array contains junk 
    for (int i = 0; i < list.count; i++) {
        strcat(result, GetFileNameWithoutExt(list.paths[i]));
        if (i + 1 < list.count) strcat(result, ";");
    }

    return result;
}

char* GetListElementByIndex(const char* list, int index, int* length) {
    char* temp = (char*)list;

    while (index--) {
        temp += TextFindIndex(temp, ";");
    }

    if (*temp == ';') temp++;

    int end = TextFindIndex(temp, ";");

    *length = (end == -1)? TextLength(temp) : end;

    return temp;
}