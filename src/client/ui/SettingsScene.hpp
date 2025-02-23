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
    Button* m_keySelect;
    bool m_autoResolution;
    
public:
    SettingsScene();

    void update() override;
};