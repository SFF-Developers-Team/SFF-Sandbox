#include <ui/SettingsScene.hpp>
#include <ui/nodes/ToggleButton.hpp>
#include <ui/nodes/DropDown.hpp>
#include <ui/nodes/Slider.hpp>
#include <format>

SettingsScene::SettingsScene() : MenuBase(), m_keySelect(nullptr), m_autoResolution(true) {
    auto stm = SettingsManager::get();
    auto game = Game::get();

    std::vector<std::string> containers{"Video", "Audio", "Keyboard"};

    auto container = std::make_shared<Container>();
    container->setSize({500, 250});
    container->setTag("center-settings");
    container->setScale(game->getGuiScale());
    addChild(container);

    auto apply = std::make_shared<Button>("Apply", [stm, container, game](Button*) {
        container->setScale(game->getGuiScale());
        stm->save();
    });

    apply->setPos({container->getWidth() / 2, container->getHeight() - container->getBorderWidth() * 2});
    apply->setAnchorY(1.f);
    apply->setWidth(container->getWidth() / containers.size());
    container->addChild(apply);

    Vec2f categorySize = {container->getWidth() / containers.size(), container->getHeight() - apply->getHeight() - apply->getBorderWidth() * 3};
    float x = 0.f;

    for(auto& s : containers) {
        auto cat = std::make_shared<Container>();
        cat->setAnchor({0.f, 0.f});
        cat->setPos({x, 0.f});
        cat->setSize(categorySize);
        container->addChild(cat);

        x += categorySize.x;

        auto title = std::make_shared<Text>("boldfont", s);
        title->setAnchorY(0.f);
        title->setPos({categorySize.x / 2, container->getBorderWidth()});
        cat->addChild(title);

        std::transform(s.begin(), s.end(), s.begin(), [](auto c) { 
            return std::tolower(c); 
        });

        cat->setTag("container-" + s);
        title->setTag(s + "-title");
    }

    // video settings
    {
        auto video = container->getChild<Container>("container-video");
        auto const elementWidth = video->getWidth() - video->getBorderWidth() * 4;

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
        
        resolutions->setPos({video->getWidth() / 2, 50.f});
        resolutions->setWidth(elementWidth);
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
        
        fullscreen->setPos({video->getWidth() / 2, resolutions->getY() + resolutions->getHeight() + resolutions->getBorderWidth()});
        fullscreen->setWidth(elementWidth);
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

        vsync->setPos({video->getWidth() / 2, fullscreen->getY() + fullscreen->getHeight() + fullscreen->getBorderWidth()});
        vsync->setWidth(elementWidth);
        video->addChild(vsync);

        auto scale = std::make_shared<Button>("GUI Scale: " + std::to_string(game->getGuiScale()), [this, stm, game](Button* btn) {
            int maxScale = std::min(getWidth() / 640, getHeight() / 360);
            int curScale = game->getGuiScale();
            
            curScale++;

            if(game->getGuiScale() >= maxScale) {
                curScale = 1;
            }

            btn->setText("GUI Scale: " + std::to_string(curScale));
            stm->setValue("video.scale", curScale);
            game->setGuiScale(curScale);
        });

        scale->setPos({video->getWidth() / 2, vsync->getY() + vsync->getHeight() + vsync->getBorderWidth()});
        scale->setWidth(elementWidth);
        video->addChild(scale);
    }

    // audio settings
    {
        auto audio = container->getChild<Container>("container-audio");
        auto const elementWidth = audio->getWidth() - audio->getBorderWidth() * 4;

        auto volume = std::make_shared<Slider<float>>("General volume: ", 0.f, 1.f, [stm](auto, auto value) {
            SetMasterVolume(value);
            stm->setValue("audio.volume.general", value);
        });

        volume->setValue(stm->getValue<float>("audio.volume.general", 0.5f));
        volume->setPos({audio->getWidth() / 2, 50.f});
        volume->setWidth(elementWidth);
        audio->addChild(volume);

        auto music = std::make_shared<Slider<float>>("Music volume: ", 0.f, 1.f, [stm](auto, auto value) {
            SoundManager::get()->setMusicVolume(value);
            stm->setValue("audio.volume.music", value);
        });
        music->setValue(stm->getValue<float>("audio.volume.music", 0.5f));
        music->setPos({audio->getWidth() / 2, volume->getY() + volume->getHeight() + volume->getBorderWidth()});
        music->setWidth(elementWidth);
        audio->addChild(music);

        auto sound = std::make_shared<Slider<float>>("Sound volume: ", 0.f, 1.f, [stm](auto, auto value) {
            SoundManager::get()->setSoundVolume(value);
            stm->setValue("audio.volume.sound", value);
        });
        sound->setValue(stm->getValue<float>("audio.volume.sound", 0.5f));
        sound->setPos({audio->getWidth() / 2, music->getY() + music->getHeight() + music->getBorderWidth()});
        sound->setWidth(elementWidth);
        audio->addChild(sound);
    }

    // keyboard settings
    {
        auto keyboard = container->getChild<Container>("container-keyboard");
        auto const elementWidth = keyboard->getWidth() - keyboard->getBorderWidth() * 4;

        auto keysContainer = std::make_shared<Container>();
        keysContainer->setPos({keyboard->getWidth() / 2, 40.f});
        keysContainer->setAnchorY(0.f);
        keysContainer->setSize({elementWidth, keyboard->getHeight() - keysContainer->getY() - keyboard->getBorderWidth() * 2});
        keyboard->addChild(keysContainer);

        auto y = keysContainer->getBorderWidth() * 2;

        for(auto& action : stm->getKeyActions()) {
            auto text = std::make_shared<Text>("font", action);
            text->setWidth(keyboard->getWidth() / 2);
            text->setPos({keyboard->getBorderWidth() * 2, y});
            text->setAnchor({0.f, 0.f});
            text->setAlignH(TextAlignmentH::H_LEFT);
            keysContainer->addChild(text);

            auto button = std::make_shared<Button>(SettingsManager::getKeyName(stm->getKeybind(action)), [this, stm](Button* btn) { 
                if(m_keySelect != nullptr) {
                    m_keySelect->setText(SettingsManager::getKeyName(stm->getKeybind(m_keySelect->getTag())));
                    m_keySelect = btn;
                }
                
                m_keySelect = btn; 
                btn->setText("...");
            });
            button->setTag(action);
            button->setWidth(100.f);
            button->setAnchor({1.f, 0.f});
            button->setPos({keysContainer->getWidth() - keyboard->getBorderWidth() * 2, y});
            keysContainer->addChild(button);

            text->setHeight(button->getHeight());

            y += button->getHeight() + button->getBorderWidth();
        }
    }
}

void SettingsScene::update() {
    auto key = GetKeyPressed();
    auto stm = SettingsManager::get();

    if(m_keySelect != nullptr && key == KEY_ESCAPE) {
        m_keySelect->setText(SettingsManager::getKeyName(stm->getKeybind(m_keySelect->getTag())));
        m_keySelect = nullptr;
        return;
    }

    if(m_keySelect != nullptr && key > 0) {
        stm->setKeybind(m_keySelect->getTag(), key);
        m_keySelect->setText(SettingsManager::getKeyName(key));
        m_keySelect = nullptr;
    }

    MenuBase::update();
}