#include <Platform.hpp>

#if defined(__linux__)
#include <cstdlib>

std::filesystem::path getHomeDir() {
    auto path = getenv("HOME");

    std::filesystem::path ret = path;
    ret /= ".SandboxForFriends";

    return ret;
}
#endif