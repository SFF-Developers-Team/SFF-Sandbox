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
    std::string m_selectKey;
    bool m_selectMode = false;
    
public:
    SettingsScene();

    void draw() override;
    void update() override;
};