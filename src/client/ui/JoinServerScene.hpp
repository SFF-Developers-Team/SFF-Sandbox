#pragma once
#include <ui/MenuBase.hpp>

class JoinServerScene : public MenuBase {
private:
    std::string m_message;
    float m_startTime = 0;
    float m_dotTime = 0;
    uint8_t m_dots = 0;

public:
    JoinServerScene(std::string const& hostname, uint16_t port);

    void draw();
    void update();
};