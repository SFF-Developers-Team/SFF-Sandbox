#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <stdint.h>
#include <stdio.h>

struct Stream;
struct Stream {
    void(*Write)(struct Stream* s, void* data, int size);
    void(*Read)(struct Stream* s, void* dest, int size);
    void(*Close)(struct Stream* s);

    union {
        struct {uint8_t* data; int size; int pos; } buf;
        FILE* file;
    };
};

int Stream_OpenFile(struct Stream* s, const char* path);
int Stream_CreateFile(struct Stream* s, const char* path);
int Stream_Allocate(struct Stream* s, int size);
void Stream_OpenFromBuffer(struct Stream* s, void* data, int size);

void Stream_FileRead(struct Stream* s, void* data, int size);
void Stream_FileWrite(struct Stream* s, void* data, int size);
void Stream_FileClose(struct Stream* s);


void Stream_BufRead(struct Stream* s, void* data, int size);
void Stream_BufWrite(struct Stream* s, void* data, int size);
void Stream_BufClose(struct Stream* s);

void Stream_Write8(struct Stream* s, int8_t num);
void Stream_Write16(struct Stream* s, int16_t num);
void Stream_Write32(struct Stream* s, int32_t num);
void Stream_Write64(struct Stream* s, int64_t num);

int8_t Stream_Read8(struct Stream* s);
int16_t Stream_Read16(struct Stream* s);
int32_t Stream_Read32(struct Stream* s);
int64_t Stream_Read64(struct Stream* s);

#endif