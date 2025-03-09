#include <Platform.hpp>

#if defined(_WIN32)
#include <Windows.h>
#include <ShlObj.h>

std::filesystem::path getHomeDir() {
    char path[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path);

    std::filesystem::path ret = path;
    ret /= "SandboxForFriends";

    return ret;
}
#endif