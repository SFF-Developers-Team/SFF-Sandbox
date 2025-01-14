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


    DrawRectangleRec({x, y, container.x, container.y}, BLUE);
    DrawRectangleLinesEx({x, y, container.x, container.y}, 5.f, DARKBLUE);

    DrawLineEx({x + container.x / 3.f, y}, {x + container.x / 3.f, y + container.y}, 5.f, DARKBLUE);
    DrawLineEx({x + container.x / 1.5f, y}, {x + container.x / 1.5f, y + container.y}, 5.f, DARKBLUE);

    drawText("Audio", {x + container.x / 3 - container.x / 6, y + 10}, 25.f, true, COL_DARKBLUE);
    drawText("Video", {x + container.x / 3 + container.x / 6, y + 10}, 25.f, true, COL_DARKBLUE);
    drawText("Keyboard", {x + container.x / 1.5f + container.x / 6, y + 10}, 25.f, true, COL_DARKBLUE);

    auto volume = stm->getValue<float>("Audio/volume", 0.5f);
    drawSlider({x + 150, y + 50, 200, 25}, "Master volume", volume, 0.f, 1.f, true, [stm](float value) {
        stm->setValue("Audio/volume", value);
        SetMasterVolume(value);
    });

    auto music = stm->getValue<float>("Audio/music", 0.5f);
    drawSlider({x + 150, y + 85, 200, 25}, "Music volume", music, 0.f, 1.f, true, [sm](float value) {
        sm->setMusicVolume(value);
    });

    auto sound = stm->getValue<float>("Audio/sound", 0.5f);
    drawSlider({x + 150, y + 125, 200, 25}, "Sound volume", sound, 0.f, 1.f, true, [sm](float value) {
        sm->setSoundVolume(value);
    });

    // drawButton(std::format("Fullscreen: {}", IsWindowFullscreen() ? "ON" : "OFF"), {m_container.x + 10, 500, 200, 50}, [&]() {
    //     int display = GetCurrentMonitor();
    //     if (!IsWindowFullscreen()) {
    //         SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    //         ToggleFullscreen();
    //     } else {
    //         ToggleFullscreen();
    //         SetWindowSize(1280, 720);
    //     }
    // });

    // drawButton("V-Sync", {m_container.x + 10, 570, 200, 50}, [&]() {
    //     if (IsWindowState(FLAG_VSYNC_HINT))
    //         ClearWindowState(FLAG_VSYNC_HINT);
    //     else
    //         SetWindowState(FLAG_VSYNC_HINT);
    // });
    // static int m_count = 0;
    // drawSliderBar({m_container.x + 350, 280, 200, 50}, "", "GUI Scale", &m_soundVolume, 0, 1);
    // DrawText("Resolutions", m_container.x + 430, 350, 25, WHITE);
    // GuiListView({m_container.x + 350, 370, 300, 250}, m_dropText.c_str(), &m_count, &m_isActive);

    // // drawDropDownBox(m_dropText, {0, 0, 200, 20}, m_count, m_isActive);
    // // if(CheckCollisionPointRec(GetMousePosition(), {0, 0, 200, 20}) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    // //     m_isActive = !m_isActive;
    // // }
    // if (IsKeyPressed(KEY_SPACE) && m_isActive != -1) {
    //     SetWindowSize(stm->getModes().at(m_isActive).width, stm->getModes().at(m_isActive).height);
    // }
    // for (int i = 0; i < stm->getBindings().size(); i++) {
    //     static int test = 0;
    //     logD("X {}", m_keyX);
    //     logD("Y {}", m_keyY);
    //     DrawText(TextFormat("%d", stm->getKeyFromID(KeyID(i))), m_keyX + 120, m_container.y + 55 * i, 40, WHITE);
    //     drawButton(stm->getKeyNameFromID(KeyID(i)), {m_keyX, m_container.y + 55 * i, 100, 50}, [&]() {
    //         m_selectMode = true;
    //         m_selectKey = KeyID(i);
    //     });
    //     count++;
    // }
    // for(auto& bind : stm->getBindings()) {

    // }
}