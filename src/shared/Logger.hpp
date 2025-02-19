#pragma once
#include <format>
#include <iostream>

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