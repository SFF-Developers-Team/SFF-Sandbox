#pragma once
#include <vector>
#include <string>

class Debug {
private:
    static inline std::vector<std::string> m_debugList;

public:
    static inline bool m_debug = false;

    static void draw();
    static void addString(std::string str);
};