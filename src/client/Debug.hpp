#pragma once
#include <vector>
#include <format>
#include <memory>

class Debug {
private:
    std::vector<std::string> m_debugList;
    bool m_visible = false;

public:
    static auto get() {
        static auto debug = std::make_shared<Debug>();
        return debug;
    }

    void draw();
    void setVisible(bool);
    bool isVisible();

    template <typename... Args>
    int addString(std::format_string<Args...> s, Args&&... args) {
        m_debugList.push_back(std::format(s, std::forward<Args>(args)...));
        return m_debugList.size() - 1;
    }

    template <typename... Args>
    void updateString(int index, std::format_string<Args...> s, Args&&... args) {
        m_debugList[index] = std::format(s, std::forward<Args>(args)...);
    }

    void removeString(int index);
};