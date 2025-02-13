#include <ui/SettingsScene.hpp>
#include <raygui.h>
#include <format>
#include <ui/nodes/DropDown.hpp>
#include <ui/nodes/ToggleButton.hpp>
#include <ui/nodes/Slider.hpp>

SettingsScene::SettingsScene() : MenuBase(), m_keySelect(nullptr), m_autoResolution(true) {
    auto stm = SettingsManager::get();

    auto container = std::make_shared<Container>();
    container->setSize({1000, 500});
    container->setPos({getWidth() / 2, getHeight() / 2 + 100.f});
    addChild(container);

    std::vector<std::string> containers{"Video", "Audio", "Keyboard"};
    Vec2f categorySize = {container->getWidth() / containers.size(), container->getHeight() - 55.f};
    float x = 0.f;

    for(auto& s : containers) {
        auto cat = std::make_shared<Container>();
        cat->setAnchor({0.f, 0.f});
        cat->setPos({x, 0.f});
        cat->setSize(categorySize);
        container->addChild(cat);

        x += categorySize.x;

        auto title = std::make_shared<Text>("boldfont", s, 40.f);
        title->setSize({categorySize.x, 60.f});
        title->setPos({categorySize.x / 2, 30.f});
        cat->addChild(title);

        std::transform(s.begin(), s.end(), s.begin(), [](auto c) { 
            return std::tolower(c); 
        });

        cat->setTag("container-" + s);
        title->setTag(s + "-title");
    }

    auto apply = std::make_shared<Button>("Apply", [stm](Button*) {
        stm->save();
    });
    apply->setSize({400.f, 40.f});
    apply->setPos({container->getWidth() / 2, container->getHeight() - 30.f});
    container->addChild(apply);

    // video settings
    {
        auto video = container->getChild<Container>("container-video");
        Vec2f const elementSize = {video->getWidth() - video->getBorderWidth() * 4, 40.f};

        std::vector<std::string> modesList{"Auto"};
        auto modes = stm->getModes();

        for(auto& mode : modes) {
            modesList.push_back(std::format("{}x{}", mode.width, mode.height));
        }

        auto resolutions = std::make_shared<DropDown>(modesList, [stm, this](DropDown*, int i) {
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

            stm->setValue("video.resolution", i);
        });
        
        resolutions->setPos({video->getWidth() / 2, 90.f});
        resolutions->setSize(elementSize);
        video->addChild(resolutions);

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

            stm->setValue("video.fullscreen", i);
        });
        
        fullscreen->setPos({video->getWidth() / 2, resolutions->getY() + 50.f});
        fullscreen->setSize(elementSize);
        video->addChild(fullscreen);

        auto vsync = std::make_shared<ToggleButton>("VSYNC", [stm](ToggleButton*, bool flag) {
            if(!flag && IsWindowState(FLAG_VSYNC_HINT)) {
                ClearWindowState(FLAG_VSYNC_HINT);
            }

            if(flag && !IsWindowState(FLAG_VSYNC_HINT)) {
                SetWindowState(FLAG_VSYNC_HINT);
            }

            stm->setValue("video.vsync", flag);
        });

        vsync->setPos({video->getWidth() / 2, fullscreen->getY() + 50.f});
        vsync->setSize(elementSize);
        video->addChild(vsync);

        auto scale = std::make_shared<Slider<int>>("GUI Scale: ", 1, 4, [stm](auto, auto value) {
            stm->setValue("video.scale", value);
        });

        scale->setPos({video->getWidth() / 2, vsync->getY() + 50.f});
        scale->setSize(elementSize);
        video->addChild(scale);
    }

    // audio settings
    {
        auto audio = container->getChild<Container>("container-audio");
        Vec2f const elementSize = {audio->getWidth() - audio->getBorderWidth() * 4, 40.f};

        auto volume = std::make_shared<Slider<float>>("General volume: ", 0.f, 1.f, [stm](auto, auto value) {
            SetMasterVolume(value);
            stm->setValue("audio.volume.general", value);
        });

        volume->setValue(stm->getValue<float>("audio.volume.general", 0.5f));
        volume->setPos({audio->getWidth() / 2, 90.f});
        volume->setSize(elementSize);
        audio->addChild(volume);

        auto music = std::make_shared<Slider<float>>("Music volume: ", 0.f, 1.f, [stm](auto, auto value) {
            SoundManager::get()->setMusicVolume(value);
            stm->setValue("audio.volume.music", value);
        });
        music->setValue(stm->getValue<float>("audio.volume.music", 0.5f));
        music->setPos({audio->getWidth() / 2, volume->getY() + 50.f});
        music->setSize(elementSize);
        audio->addChild(music);

        auto sound = std::make_shared<Slider<float>>("Sound volume: ", 0.f, 1.f, [stm](auto, auto value) {
            SoundManager::get()->setSoundVolume(value);
            stm->setValue("audio.volume.sound", value);
        });
        sound->setValue(stm->getValue<float>("audio.volume.sound", 0.5f));
        sound->setPos({audio->getWidth() / 2, music->getY() + 50.f});
        sound->setSize(elementSize);
        audio->addChild(sound);
    }

    // keyboard settings
    {
        auto keyboard = container->getChild<Container>("container-keyboard");
        Vec2f const elementSize = {keyboard->getWidth() - keyboard->getBorderWidth() * 4, 40.f};

        auto keysContainer = std::make_shared<Container>();
        keysContainer->setPos({keyboard->getWidth() / 2, 60.f});
        keysContainer->setAnchorY(0.f);
        keysContainer->setSize({elementSize.x, keyboard->getHeight() - keysContainer->getY() - keyboard->getBorderWidth() * 2});
        keyboard->addChild(keysContainer);

        auto y = 30.f;

        for(auto& action : stm->getKeyActions()) {
            auto text = std::make_shared<Text>("font", action, 40.f);
            text->setSize({keyboard->getWidth() / 2, 40.f});
            text->setPos({10.f, y});
            text->setAnchorX(0.f);
            text->setAlignH(TextAlignmentH::H_LEFT);
            keysContainer->addChild(text);

            auto button = std::make_shared<Button>(SettingsManager::getKeyName(stm->getKeybind(action)), [this](Button* btn) { 
                m_keySelect = btn; 
                btn->setText("...");
            });
            button->setTag(action);
            button->setSize({200.f, 40.f});
            button->setAnchorX(1.f);
            button->setPos({keysContainer->getWidth() - 10.f, y});
            keysContainer->addChild(button);

            y += 45.f;
        }
    }
}

void SettingsScene::update() {
    auto key = GetKeyPressed();
    auto stm = SettingsManager::get();

    if(m_keySelect != nullptr && key == KEY_ESCAPE) {
        m_keySelect->setText(SettingsManager::getKeyName(stm->getKeybind(m_keySelect->getTag())));
        m_keySelect = nullptr;
    }

    if(m_keySelect != nullptr && key > 0) {
        stm->setKeybind(m_keySelect->getTag(), key);
        m_keySelect->setText(SettingsManager::getKeyName(key));
        m_keySelect = nullptr;
    }

    MenuBase::update();
}