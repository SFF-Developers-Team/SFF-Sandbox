#pragma once
#include <ui/MenuBase.hpp>
#include <raylib.h>

class MultiplayerScene : public MenuBase {
private:
    std::string m_username;
    std::string m_hostname;

public:
    MultiplayerScene();
    ~MultiplayerScene();

    void draw() override;
};