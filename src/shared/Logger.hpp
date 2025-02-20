#pragma once
#include <format>
#include <iostream>

#ifdef PLATFORM_DESKTOP
template <typename... Args>
void logD(std::format_string<Args...> s, Args&&... args) {
    std::printf("[DBG] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args>
void logW(std::format_string<Args...> s, Args&&... args) {
    std::printf("[WRN] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args>
void logE(std::format_string<Args...> s, Args&&... args) {
    std::printf("[ERR] %s\n", std::format(s, std::forward<Args>(args)...).c_str());
}

template <typename... Args>
void logM(std::format_string<Args...> s, Args&&... args) {
    std::printf("%s\n", std::format(s, std::forward<Args>(args)...).c_str());
}
#elif defined(PLATFORM_ANDROID)
template <typename... Args>
void logD(Args&&... args) { /*NOT IMPLEMENTED*/ }

template <typename... Args>
void logW(Args&&... args) { /*NOT IMPLEMENTED*/ }

template <typename... Args>
void logE(Args&&... args) { /*NOT IMPLEMENTED*/ }

template <typename... Args>
void logM(Args&&... args) { /*NOT IMPLEMENTED*/ }
    
#endif