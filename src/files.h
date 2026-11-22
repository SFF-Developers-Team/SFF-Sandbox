#ifndef SAVES_H
#define SAVES_H
#include "raylib.h"

char* GetDataDirectory(void);
char* GetDataSubdirectory(const char* subdir);

void InitFiles(void);

const char* JoinFilePathListFilenames(FilePathList list);
// Find position and length of substring in ";" separated list
char* GetListElementByIndex(const char* list, int index, int* length);
#endif