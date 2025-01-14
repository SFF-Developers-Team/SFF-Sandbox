#pragma once
#include <format>
#include <map>
#include <memory>


enum DebugID : uint8_t {
    GAME_VERSION,
    FPS,

    RENDER_CHUNKS,
    RENDER_BLOCKS,
    RENDER_PLAYERS,

    WORLD_TIME_SPENT,

    PLAYER_POSITION,
    PLAYER_TARGET_BLOCK
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
    void setString(DebugID id, std::format_string<Args...> s, Args&&... args) {
        m_debugList[id] = std::format(s, std::forward<Args>(args)...);
    }

    void removeString(DebugID id);
};