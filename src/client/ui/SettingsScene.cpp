#include <ui/SettingsScene.hpp>
#include <raygui.h>
#include <format>
#include <ui/nodes/DropDown.hpp>
#include <ui/nodes/ToggleButton.hpp>

SettingsScene::SettingsScene() : MenuBase() {
    auto stm = SettingsManager::get();

    auto container = std::make_shared<Container>();
    container->setSize({1000, 500});
    container->setPos({getWidth() / 2, getHeight() / 2 + 100.f});
    addChild(container);

    Vec2f categorySize = {container->getWidth() / 3, container->getHeight()};

    auto video = std::make_shared<Container>();
    video->setSize(categorySize);
    video->setPos({categorySize.x / 2, categorySize.y / 2});
    container->addChild(video);

    auto audio = std::make_shared<Container>();
    audio->setSize(categorySize);
    audio->setPos({categorySize.x * 1.5f, categorySize.y / 2});
    container->addChild(audio);

    auto keyboard = std::make_shared<Container>();
    keyboard->setSize(categorySize);
    keyboard->setPos({categorySize.x * 2.5f, categorySize.y / 2});
    container->addChild(keyboard);

    auto videoTitle = std::make_shared<Text>("boldfont", "Video", 40.f);
    videoTitle->setSize({categorySize.x, 60.f});
    videoTitle->setPos({categorySize.x / 2, 30.f});
    video->addChild(videoTitle);

    auto audioTitle = std::make_shared<Text>("boldfont", "Audio", 40.f);
    audioTitle->setSize({categorySize.x, 60.f});
    audioTitle->setPos({categorySize.x / 2, 30.f});
    audio->addChild(audioTitle);

    auto keyboardTitle = std::make_shared<Text>("boldfont", "Keyboard", 40.f);
    keyboardTitle->setSize({categorySize.x, 60.f});
    keyboardTitle->setPos({categorySize.x / 2, 30.f});
    keyboard->addChild(keyboardTitle);

    std::vector<std::string> modesList{"Auto"};
    auto modes = stm->getModes();

    for(auto& mode : modes) {
        modesList.push_back(std::format("{}x{}", mode.width, mode.height));
    }

    auto dropdown = std::make_shared<DropDown>(modesList, [stm, this](DropDown*, int i) {
        auto monitor = GetCurrentMonitor();
        auto modes = stm->getModes();
        auto game = Game::get();
        auto ws = game->getLastWindowSize();
        auto mode = (IsWindowState(FLAG_FULLSCREEN_MODE) ? VideoMode {GetMonitorWidth(monitor), GetMonitorHeight(monitor)} : ws.to<VideoMode>());

        if(i > 0) {
            mode = modes[i - 1];
        }
    
        m_autoResolution = i == 0;

        SetWindowSize(mode.width, mode.height);
        SetWindowPosition((GetMonitorWidth(monitor) - mode.width) / 2, (GetMonitorHeight(monitor) - mode.height) / 2);
    });
    
    dropdown->setPos({video->getWidth() / 2, 90.f});
    dropdown->setSize({video->getWidth() - video->getBorderWidth() * 4, 40.f});
    video->addChild(dropdown);

    auto fullscreen = std::make_shared<DropDown>(std::vector<std::string>{"Window", "Fullscreen", "Borderless"}, [stm, this](DropDown*, int i) {
        auto size = Game::get()->getLastWindowSize();
        auto mon = GetCurrentMonitor();

        switch (i) {
            case 0:
                ClearWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
                ClearWindowState(FLAG_FULLSCREEN_MODE);
                SetWindowSize(size.x, size.y);
                break;
            case 1:
                ClearWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
                SetWindowState(FLAG_FULLSCREEN_MODE);
                
                if(m_autoResolution) {
                    SetWindowSize(GetMonitorWidth(mon), GetMonitorHeight(mon));
                }
                break;
            case 2:
                ClearWindowState(FLAG_FULLSCREEN_MODE);
                SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
                break;
        }
    });
    
    fullscreen->setPos({video->getWidth() / 2, dropdown->getY() + 50.f});
    fullscreen->setSize({video->getWidth() - video->getBorderWidth() * 4, 40.f});
    video->addChild(fullscreen);

    auto vsync = std::make_shared<ToggleButton>("VSYNC", [](ToggleButton*, bool flag) {
        if(!flag && IsWindowState(FLAG_VSYNC_HINT)) {
            ClearWindowState(FLAG_VSYNC_HINT);
        }

        if(flag && !IsWindowState(FLAG_VSYNC_HINT)) {
            SetWindowState(FLAG_VSYNC_HINT);
        }
    });

    vsync->setPos({video->getWidth() / 2, fullscreen->getY() + 50.f});
    vsync->setSize({video->getWidth() - video->getBorderWidth() * 4, 40.f});
    video->addChild(vsync);
}

void SettingsScene::update() {
    MenuBase::update();
    // auto stm = SettingsManager::get();
    // auto key = GetKeyPressed();

    // if (m_selectMode && key > 0) {
    //     stm->setKeybind(m_selectKey, key);
    //     m_selectMode = false;
    // }
}

void SettingsScene::draw() {
    MenuBase::draw();

    // auto sm = SoundManager::get();
    // auto stm = SettingsManager::get();
    // auto game = Game::get();
    // auto player = Game::get()->getPlayer();
    // Vec2f const scr = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    // Vec2f const container = {1235.f, 400.f};
    // float const x = scr.x / 2 - container.x / 2;
    // float const y = 100 + scr.y / 2 - container.y / 2;

    // auto volume = stm->getValue<float>("Audio/volume", 0.5f);
    // drawSlider({x + 150, y + 50, 200, 25}, "Master volume", volume, 0.f, 1.f, true, [stm](float value) {
    //     stm->setValue("Audio/volume", value);
    //     SetMasterVolume(value);
    // });

    // auto music = stm->getValue<float>("Audio/music", 0.5f);
    // drawSlider({x + 150, y + 85, 200, 25}, "Music volume", music, 0.f, 1.f, true, [sm](float value) {
    //     sm->setMusicVolume(value);
    // });

    // auto sound = stm->getValue<float>("Audio/sound", 0.5f);
    // drawSlider({x + 150, y + 125, 200, 25}, "Sound volume", sound, 0.f, 1.f, true, [sm](float value) {
    //     sm->setSoundVolume(value);
    // });

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