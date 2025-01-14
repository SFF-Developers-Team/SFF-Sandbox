#pragma once
#include <ui/MenuBase.hpp>
#include <entity/Player.hpp>
#include <SoundManager.hpp>
#include <SettingsManager.hpp>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <toml.hpp>
#include <string>

class SettingsScene : public MenuBase {
private:
    std::string m_dropText;

    // bool m_fullscreen = 0;
    bool m_selectMode = 0;
    // int m_isActive = -1;
    std::string m_selectKey;
    // std::vector<int> poss;
    // float m_keyX = m_container.x + 700;
    // float m_keyY = 480;

    // int count = 0;

public:
    SettingsScene();

    void draw() override;
    void update() override;
};