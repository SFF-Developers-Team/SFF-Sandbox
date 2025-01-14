#pragma once
#include <ui/MenuBase.hpp>
#include <entity/Player.hpp>
#include <SoundManager.hpp>
#include <SettingsManager.hpp>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <toml.hpp>

class SettingsScene : public MenuBase {
private:
    float m_volume = 1;
    float m_musicVolume = 1;
    float m_soundVolume = 1;

    Rectf m_container = {20, 270, 1235, 400};
    Rectf m_containerBG = {30, 260, 1255, 420};

    toml::v3::table config;

    std::string_view defaultConfig = R"(# Auto-generated config for SFFSandbox settings
    volume=1
    musicVolume=1)";
    std::string m_dropText = "";

    bool m_fullscreen = 0;
    bool m_selectMode = 0;
    int m_isActive = -1;
    KeyID m_selectKey;
    std::vector<int> poss;
    float m_keyX = m_container.x + 700;
    float m_keyY = 480;

    int count = 0;

public:
    SettingsScene();
    ~SettingsScene();

    void draw() override;
};