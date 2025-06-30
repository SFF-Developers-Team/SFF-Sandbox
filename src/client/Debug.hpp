#pragma once

#include <string>
#include <vector>

class Debug {
private:
    std::vector<std::string> m_info;
public:
    static inline Debug& Get() {
        static Debug dbg;
        return dbg;
    }
    Debug() {};
    void Draw();
    void AddString(std::string info) { m_info.push_back(info.append("\n")); }
};