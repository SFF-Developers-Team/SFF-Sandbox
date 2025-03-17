#pragma once
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOGDI
    #define NOUSER
    #define NOSOUND
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    // Type required before windows.h inclusion
    typedef struct tagMSG *LPMSG;

    #include <raylib.h>
    #include <windows.h>
    #include <enet.h>
#endif