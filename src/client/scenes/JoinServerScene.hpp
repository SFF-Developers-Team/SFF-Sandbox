#pragma once
#include <scenes/MenuBase.hpp>
#include <ui/basic/Button.hpp>
#include <ui/basic/Text.hpp>

class JoinServerScene : public MenuBase {
private:
    std::string m_message;
    float m_startTime = 0;
    float m_dotTime = 0;
    uint8_t m_dots = 0;
    
    std::shared_ptr<Text> m_state;
    std::shared_ptr<Button> m_cancelBtn;

public:
    JoinServerScene(std::string const& hostname, uint16_t port);
    void update();
};