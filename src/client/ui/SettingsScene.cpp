#include <ui/SettingsScene.hpp>
#include <ui/nodes/SettingsToggle.hpp>
#include <ui/nodes/DropDown.hpp>
#include <ui/nodes/SettingsSlider.hpp>
#include <ui/nodes/ListContainer.hpp>
#include <SoundManager.hpp>
#include <SettingsManager.hpp>
#include <ui/nodes/List.hpp>
#include <format>
#include <Logger.hpp>

SettingsScene::SettingsScene() : MenuBase(), m_keySelect(nullptr), m_autoResolution(true) {
    auto stm = SettingsManager::get();
    auto game = Game::get();

    auto container = std::make_shared<Container>();
    container->setPos(getSize() / 2);
    container->setSize({350, 200});
    container->setFlags(FLAG_ALWAYS_CENTER | FLAG_GUI_SCALE);
    addChild(container);

    // auto back = std::make_shared<Button>("Back", [this](Button*) { destroy(); });
    // back->setPos({container->getWidth() / 2, container->getHeight() - container->getBorderWidth() * 2});
    // back->setAnchorY(1.f);
    // back->setWidth(container->getWidth() / containers.size());
    // container->addChild(back);

    std::vector<std::string> containers{"Video", "Audio", "Keyboard"};

    auto categoryList = std::make_shared<List>(containers, [this](auto, int selected) {
        for (auto i = 0; i < m_categories.size(); i++) {
            m_categories[i]->setVisible(i == selected);
            m_categories[i]->setEnabled(i == selected);
        }
    });

    categoryList->setPos({container->getBorderWidth() * 2, container->getHeight() / 2});
    categoryList->setSize({container->getWidth() / 3 - container->getBorderWidth() * 4, container->getHeight() - container->getBorderWidth() * 4});
    categoryList->setAnchor({0.f, 0.5f});
    container->addChild(categoryList);

    for(auto& s : containers) {
        auto cat = std::make_shared<ListContainer>(false);
        cat->setAnchor({0.f, 0.5f});
        cat->setPos({container->getBorderWidth() * 3 + categoryList->getWidth(), container->getHeight() / 2});
        cat->setSize({container->getWidth() - cat->getX() - container->getBorderWidth() * 2, categoryList->getHeight()});
        cat->addChild(std::make_shared<Text>("boldfont", s));
        cat->setVisible(false);
        cat->setEnabled(false);
        container->addChild(cat);
        m_categories.push_back(cat);

        std::transform(s.begin(), s.end(), s.begin(), [](auto c) { 
            return std::tolower(c); 
        });

        cat->setTag("container-" + s);
    }

    m_categories[0]->setVisible(true);
    m_categories[0]->setEnabled(true);

    // video settings
    {
        auto video = container->getChild<ListContainer>("container-video");
        float const elementWidth = video->getWidth() - video->getBorderWidth() * 4;

        /*
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
        */

#ifdef PLATFORM_DESKTOP
        video->addChild(std::make_shared<SettingsToggle>("video.fullscreen", "Fullscreen", [stm, this](ToggleButton*, bool flag) {
            // auto modes = stm->getModes();
            auto mon = GetCurrentMonitor();
            auto game =  Game::get();
            // auto mode = stm->getValue<int>("video.resolution", modes.size());

            Vec2i windowSize = game->getLastWindowSize();
            Vec2i fullscreenSize = {GetMonitorWidth(mon), GetMonitorHeight(mon)};

            auto states = FLAG_WINDOW_UNDECORATED;

            (flag) ? SetWindowState(states) : ClearWindowState(states);
            SetWindowSize((flag) ? fullscreenSize.x : windowSize.x, (flag) ? fullscreenSize.y : windowSize.y);
            SetWindowPosition((flag) ? 0 : (GetMonitorWidth(mon) - windowSize.x) / 2, (flag) ? 0 : (GetMonitorHeight(mon) - windowSize.y) / 2);
        

            stm->setValue("video.fullscreen", flag);
            game->updateGuiScale();
            game->checkSceneFlags(game->getScene());
            onShow();
        }));

        video->addChild(std::make_shared<SettingsToggle>("video.vsync", "VSYNC", [stm](ToggleButton*, bool flag) {
            if(!flag && IsWindowState(FLAG_VSYNC_HINT)) {
                ClearWindowState(FLAG_VSYNC_HINT);
            }

            if(flag && !IsWindowState(FLAG_VSYNC_HINT)) {
                SetWindowState(FLAG_VSYNC_HINT);
            }

            stm->setValue("video.vsync", flag);
        }));
#endif

        video->addChild(std::make_shared<Button>("GUI Scale: " + std::to_string(game->getGuiScale()), [this, stm, game](Button* btn) {
            int maxScale = std::min(getWidth() / 640, getHeight() / 360);
            int curScale = game->getGuiScale();
            
            curScale++;

            if(game->getGuiScale() >= maxScale) {
                curScale = 1;
            }

            btn->setText("GUI Scale: " + std::to_string(curScale));
            stm->setValue("video.scale", curScale);
            game->setGuiScale(curScale);
        }));
    }

    // audio settings
    {
        auto audio = container->getChild<ListContainer>("container-audio");
        auto const elementWidth = audio->getWidth() - audio->getBorderWidth() * 4;

        audio->addChild(std::make_shared<SettingsSlider<float>>("audio.volume.general", "General volume: ", 0.f, 1.f, 0.5f, [stm](auto, auto value) {
            SetMasterVolume(value);
        }));

        audio->addChild(std::make_shared<SettingsSlider<float>>("audio.volume.music", "Music volume: ", 0.f, 1.f, 1.f, [stm](auto, auto value) {
            SoundManager::get()->setMusicVolume(value);
        }));

        audio->addChild(std::make_shared<SettingsSlider<float>>("audio.volume.sound", "Sound volume: ", 0.f, 1.f, 1.f, [stm](auto, auto value) {
            SoundManager::get()->setSoundVolume(value);
        }));
    }

    // keyboard settings
    {
        auto keyboard = container->getChild<ListContainer>("container-keyboard");
        auto const elementWidth = keyboard->getWidth() - keyboard->getBorderWidth() * 4;

        auto keysContainer = std::make_shared<Container>();
        keysContainer->setAnchorY(0.f);
        keyboard->addChild(keysContainer);
        keysContainer->setHeight(keyboard->getHeight() - keysContainer->getY() - keyboard->getBorderWidth() * 2);

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
                }
                
                m_keySelect = btn; 
                btn->setText("...");

                setKeyBackEnabled(false);
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
    MenuBase::update();

    auto key = GetKeyPressed();
    auto stm = SettingsManager::get();

    if(m_keySelect != nullptr && key > 0) {
        if(key == KEY_ESCAPE) {
            key = stm->getKeybind(m_keySelect->getTag());
        } else {
            stm->setKeybind(m_keySelect->getTag(), key);
        }

        m_keySelect->setText(SettingsManager::getKeyName(key));
        m_keySelect = nullptr;

        setKeyBackEnabled(true);
    }
}