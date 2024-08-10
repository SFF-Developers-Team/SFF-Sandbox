#pragma once
#include <vector>
#include <format>

class Debug {
private:
    static inline std::vector<std::string> m_debugList;

public:
    static inline bool m_debug = false;

    static void draw();

    template <typename... Args>
    static void addString(std::format_string<Args...> s, Args&&... args) {
        if(m_debug) {
            m_debugList.push_back(std::format(s, std::forward<Args>(args)...));
        }
    }
};