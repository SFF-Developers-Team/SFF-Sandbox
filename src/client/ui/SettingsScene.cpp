#include <ui/SettingsScene.hpp>
#include <raygui.h>
SettingsScene::SettingsScene() {
    // if(!std::filesystem::exists("settings.toml")) {
    //     std::ofstream file("settings.toml");
    //     if(!file.is_open()) {
    //         std::exit(1);
    //     }

    //     file.write(defaultConfig.data(), defaultConfig.size());
    // }
    // config = toml::parse("settings.toml");

    // m_volume = config["volume"].value_or(1);
    // m_musicVolume = config["musicVolume"].value_or(1);
    auto stm = SettingsManager::get();
    auto modes = stm->getModes();
    m_bgColor = COL_SKYBLUE;
    for(auto& mode: modes) {
        m_dropText += std::to_string(mode.width).append("x").append(std::to_string(mode.height)).append(";");
    }
}
SettingsScene::~SettingsScene() {

    // m_volume = config["volume"].value_or(1);
    // m_musicVolume = config["musicVolume"].value_or(1);

    // std::ofstream file("settings.toml");
    // file << config;
}
void SettingsScene::draw() {
    MenuBase::draw();

    auto sm = SoundManager::get();
    auto stm = SettingsManager::get();
    auto game = Game::get();
    auto player = Game::get()->getPlayer();
    auto key = GetKeyPressed();

    m_containerBG.x = (float)(GetScreenWidth() - 1255) / 2;
    m_container.x = (float)(GetScreenWidth() - m_container.width) / 2;

    if(m_selectMode && key > 0) {
        stm->setSelectKey(m_selectKey, key);
        m_selectMode = false;
    }
    DrawRectangleRec(m_containerBG.to<Rectangle>(), COL_BLUE.to<Color>());

    DrawRectangleRec(m_container.to<Rectangle>(), COL_DARKBLUE.to<Color>());
    drawSliderBar({m_container.x + 10, 280, 200, 50}, "", "Volume", &m_volume, 0, 1);
    SetMasterVolume(m_volume);

    drawSliderBar({m_container.x + 10, 350, 200, 50}, "", "Music Volume", &m_musicVolume, 0, 1);
    sm->setMusicVolume(m_musicVolume);

    drawSliderBar({m_container.x + 10, 420, 200, 50}, "", "Sound Volume", &m_soundVolume, 0, 1);
    sm->setSoundVolume(m_soundVolume);  

    drawButton("FullScreen", {m_container.x + 10, 500, 150, 50}, [&]() {
        int display = GetCurrentMonitor();
        if(!IsWindowFullscreen()) {
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
            ToggleFullscreen();     
        } else {
            ToggleFullscreen();
            SetWindowSize(1280, 720);
        } 
    });
    drawButton("V-Sync", {m_container.x + 10, 570, 150, 50}, [&]() {
        if (IsWindowState(FLAG_VSYNC_HINT)) ClearWindowState(FLAG_VSYNC_HINT);
        else SetWindowState(FLAG_VSYNC_HINT);  
    });
    static int m_count = 0;
    drawSliderBar({m_container.x + 350, 280, 200, 50}, "", "GUI Scale", &m_soundVolume, 0, 1);
    DrawText("Resolutions", m_container.x + 430, 350, 25, WHITE);
    GuiListView({m_container.x + 350, 370, 300, 250}, m_dropText.c_str(), &m_count, &m_isActive);

    // drawDropDownBox(m_dropText, {0, 0, 200, 20}, m_count, m_isActive);
    // if(CheckCollisionPointRec(GetMousePosition(), {0, 0, 200, 20}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    //     m_isActive = !m_isActive;
    // }
    if(IsKeyPressed(KEY_SPACE) && m_isActive != -1) {
        SetWindowSize(stm->getModes().at(m_isActive).width, stm->getModes().at(m_isActive).height);
    }
    logD("Scroll Index: {}", m_isActive);
    for(auto& bind : stm->getBindings()) {
        // 
        // if(count > 0) {
        //     m_keyY += 60;
        // }
        // count++;

        // if(m_keyY > 1000) {
        //     m_keyX += 100;
        //     m_keyY = 420;
        //     count = 0;
        // }
        // DrawText(TextFormat("%d", stm->getKeyFromID(bind.first)), 200, 100 + 60 * bind.first, 50, WHITE);
        // drawButton(stm->getKeyNameFromID(bind.first), {m_keyX, m_keyY, 100, 50}, [&]() {
        //     m_selectMode = true;
        //     m_selectKey = bind.first;
        // });
    }
}