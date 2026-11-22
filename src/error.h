#ifndef ERROR_H
#define ERROR_H

typedef enum {
    RES_OK = 0,
    RES_OUT_OF_MEMORY = -1,
    RES_FILE_NOT_FOUND = -2,
} Result;

#endif