#pragma once
#include <format>
#include <map>
#include <memory>


enum DebugID : uint8_t {
    GAME_VERSION,
    FPS,

    CHUNKS_DRAWN,
    BLOCKS_DRAWN,
    PLAYERS_DRAWN,

    TIME_SPENT
};

class Debug {
private:
    std::map<DebugID, std::string> m_debugList;
    bool m_visible = false;

public:
    static auto get() {
        static auto debug = std::make_shared<Debug>();
        return debug;
    }

    Debug();

    void draw();
    void setVisible(bool);
    bool isVisible();

    template <typename... Args>
    void addString(DebugID id, std::format_string<Args...> s, Args&&... args) {
        m_debugList[id] = std::format(s, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void updateString(DebugID id, std::format_string<Args...> s, Args&&... args) {
        m_debugList[id] = std::format(s, std::forward<Args>(args)...);
    }

    void removeString(DebugID id);
};