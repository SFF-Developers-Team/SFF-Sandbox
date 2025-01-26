#include <ui/SettingsScene.hpp>
#include <raygui.h>
#include <format>

SettingsScene::SettingsScene() {
    m_bgColor = COL_SKYBLUE;

    auto stm = SettingsManager::get();

    for (auto& mode : stm->getModes()) {
        m_dropText.append(std::format("{}x{};", mode.width, mode.height));
    }
}

void SettingsScene::update() {
    auto stm = SettingsManager::get();
    auto key = GetKeyPressed();
    if(IsKeyPressed(KEY_ESCAPE)) {
        Game::get()->popScene();
    }
    if (m_selectMode && key > 0) {
        stm->setKeybind(m_selectKey, key);
        m_selectMode = false;
    }
}

void SettingsScene::draw() {
    MenuBase::draw();

    auto sm = SoundManager::get();
    auto stm = SettingsManager::get();
    auto game = Game::get();
    auto player = Game::get()->getPlayer();
    Vec2f const scr = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    Vec2f const container = {1235.f, 400.f};
    float const x = scr.x / 2 - container.x / 2;
    float const y = 100 + scr.y / 2 - container.y / 2;

    // Base UI
    DrawRectangleRec({x, y, container.x, container.y}, DARKBLUE);
    DrawRectangleLinesEx({x, y, container.x, container.y}, 5.f, BLUE);

    DrawLineEx({x + container.x / 3.f, y}, {x + container.x / 3.f, y + container.y}, 5.f, BLUE);
    DrawLineEx({x + container.x / 1.5f, y}, {x + container.x / 1.5f, y + container.y}, 5.f, BLUE);

    drawText("Audio", {x + container.x / 3 - container.x / 6, y + 10}, 25.f, true, COL_WHITE);
    drawText("Video", {x + container.x / 3 + container.x / 6, y + 10}, 25.f, true, COL_WHITE);
    drawText("Keyboard", {x + container.x / 1.5f + container.x / 6, y + 10}, 25.f, true, COL_WHITE);
 
    // drawButton("Back", {x + container.x / 3 + container.x / 6, y + 300, 100, 50}, [&]() {
    //     Game::get()->popScene();
    // });
    // Audio
    auto volume = stm->getValue<float>("Audio/volume", 0.5f);
    drawSlider({x + 150, y + 50, 200, 25}, "Master volume", volume, 0.f, 1.f, true, true, [stm](float value) {
        stm->setValue("Audio/volume", value);
        SetMasterVolume(value);
    });

    auto music = stm->getValue<float>("Audio/music", 0.5f);
    drawSlider({x + 150, y + 85, 200, 25}, "Music volume", music, 0.f, 1.f, true, true, [sm](float value) {
        sm->setMusicVol(value);
    });

    auto sound = stm->getValue<float>("Audio/sound", 0.5f);
    drawSlider({x + 150, y + 125, 200, 25}, "Sound volume", sound, 0.f, 1.f, true, true, [sm](float value) {
        sm->setSoundVol(value);
    });
    // Video
    GuiListView({x + 442.5f, y + 50, 350, 155}, m_dropText.c_str(), &m_count, &m_activeResolution);
    drawButton("Apply", {x + 442.5f, y + 215, 350, 30}, [&]() {
        if(m_activeResolution != -1) {
            SetWindowSize(stm->getModes().at(m_activeResolution).width, stm->getModes().at(m_activeResolution).height);
        }
    });
    auto fullscreen = stm->getValue<bool>("Video/fullscreen", false);
    drawToggleSlider({x + 442.5f, y + 250, 350, 30}, "FullScreen", fullscreen, [&]() {
        int display = GetCurrentMonitor();
        if (!fullscreen) {
            SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
            ToggleFullscreen();
        } else {
            ToggleFullscreen();
            SetWindowSize(1280, 720);
        }
        fullscreen = !fullscreen;
        stm->setValue("Video/fullscreen", fullscreen);
    });
    auto vsync = stm->getValue<int>("Video/vsync", 1);
    drawToggleSlider({x + 442.5f, y + 285, 350, 30}, "V-Sync", vsync, [&]() {
        if (IsWindowState(FLAG_VSYNC_HINT)) {
            vsync = 0;
            ClearWindowState(FLAG_VSYNC_HINT);
        } else {
            vsync = 1;
            SetWindowState(FLAG_VSYNC_HINT);
        }
        stm->setValue("Video/vsync", vsync);
    });
    auto guiScale = stm->getValue<int>("Video/guiscale", 1);
    drawSlider({x + 540.f, y + 345, 200, 25}, "GUI Scale", guiScale, 0, 2, false, false, [&](int value) {
        stm->setValue("Video/guiscale", guiScale);
    });
    // Keyboard
    for(int i = 0; i < stm->getKeyList().size(); i++) {
        drawButton(stm->getKeyList().at(i), {x + 850.f, y + 60 * i + 50, 100, 50}, [&]() {
            m_selectMode = true;
            m_selectKey = stm->getKeyList().at(i);
        });
        auto key = stm->getKeybind(stm->getKeyList().at(i));
        DrawText(stm->getKeyName(key).c_str(), x + 850.f + 110, y + 60 * i + 60, 30, WHITE);
    }
}