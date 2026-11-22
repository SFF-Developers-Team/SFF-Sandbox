#include "stream.h"
#include "raylib.h"
#include <string.h>

int Stream_OpenFile(struct Stream* s, const char* path) {
    s->file = fopen(path, "r");

    if (!s->file) return -1;

    s->Write = Stream_FileWrite;
    s->Read = Stream_FileRead;
    s->Close = Stream_FileClose;

    return 0;
}

int Stream_CreateFile(struct Stream* s, const char* path) {
    s->file = fopen(path, "w");

    if (!s->file) return -1;

    s->Write = Stream_FileWrite;
    s->Read = Stream_FileRead;
    s->Close = Stream_FileClose;

    return 0;
}

int Stream_Allocate(struct Stream* s, int size) {
    s->buf.data = MemAlloc(size);
    s->buf.size = size;

    if (!s->buf.size) return -1;

    s->Write = Stream_BufWrite;
    s->Read = Stream_BufRead;
    s->Close = Stream_BufClose;

    return 0;
}

void Stream_OpenFromBuffer(struct Stream* s, void* data, int size) {
    s->buf.data = data;
    s->buf.size = size;

    s->Write = Stream_BufWrite;
    s->Read = Stream_BufRead;
    s->Close = Stream_BufClose;
}

void Stream_FileRead(struct Stream* s, void* data, int size) {
    fread(data, size, 1, s->file);
}

void Stream_FileWrite(struct Stream* s, void* data, int size) {
    fwrite(data, size, 1, s->file);
}

void Stream_FileClose(struct Stream* s) {
    fclose(s->file);
}


void Stream_BufRead(struct Stream* s, void* data, int size) {
    memcpy(data, s->buf.data + s->buf.pos, size);
    s->buf.pos += size;
}

void Stream_BufWrite(struct Stream* s, void* data, int size) {
    memcpy(s->buf.data + s->buf.pos, data, size);
    s->buf.pos += size;
}

void Stream_BufClose(struct Stream* s) {
    MemFree(s->buf.data);
}

void Stream_Write8(struct Stream* stream, int8_t num) {
    stream->Write(stream, &num, 1);
}

void Stream_Write16(struct Stream* stream, int16_t num) {
    stream->Write(stream, &num, 2);
}

void Stream_Write32(struct Stream* stream, int32_t num) {
    stream->Write(stream, &num, 4);
}

void Stream_Write64(struct Stream* stream, int64_t num) {
    stream->Write(stream, &num, 8);
}

int8_t Stream_Read8(struct Stream* stream) {
    int8_t ret;
    stream->Read(stream, &ret, 1);

    return ret;
}

int16_t Stream_Read16(struct Stream* stream) {
    int16_t ret;
    stream->Read(stream, &ret, 2);

    return ret;
}

int32_t Stream_Read32(struct Stream* stream) {
    int32_t ret;
    stream->Read(stream, &ret, 4);

    return ret;
}

int64_t Stream_Read64(struct Stream* stream) {
    int64_t ret;
    stream->Read(stream, &ret, 8);

    return ret;
}