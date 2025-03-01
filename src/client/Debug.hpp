#pragma once
#include <format>
#include <memory>
#include <map>

class Container;

enum DebugID : uint8_t {
    GAME_VERSION,
    FPS,

    RENDER_CHUNKS,
    RENDER_BLOCKS,
    RENDER_PLAYERS,

    WORLD_TIME_SPENT,

    PLAYER_POSITION,
    PLAYER_TARGET_BLOCK,
    PLAYER_HEALTH
};

class Debug {
private:
    std::map<DebugID, std::string> m_debugList;
    std::shared_ptr<Container> m_debugConsole;
    std::string m_log;
    bool m_visible = false;

public:
    static auto get() {
        static auto debug = new Debug();
        return debug;
    }

    Debug();

    void draw();
    void update();
    void setVisible(bool flag) { m_visible = flag; }
    bool isVisible() { return m_visible; }
    void toggleVisibility() { m_visible = !m_visible; }

    template <typename... Args>
    void setString(DebugID id, std::format_string<Args...> s, Args&&... args) {
        m_debugList[id] = std::format(s, std::forward<Args>(args)...);
    }

    void removeString(DebugID id) { m_debugList.erase(id); }

    void log(std::string const& str);
    void allocateConsole();
};