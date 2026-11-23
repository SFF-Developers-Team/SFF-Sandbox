#include <raylib-cpp.hpp>
#include <scenes/MainMenuScene.hpp>
#include <scenes/MenuBase.hpp>
#include <managers/ResourceManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/World.hpp>
#include <Tilemap.hpp>
#include <Logger.hpp>
#include <Game.hpp>
#include <managers/SettingsManager.hpp>
#include <managers/StyleManager.hpp>
#include <managers/RenderManager.hpp>
#include <managers/RecipesManager.hpp>
#include <managers/Debug.hpp>
#include <net/Multiplayer.hpp>
#include <Platform.hpp>

#define CHECK_DIR(path) if (!std::filesystem::exists(path)) std::filesystem::create_directory(path);

void Game::pushScene(std::shared_ptr<Scene> scene) {
    if(m_scene != nullptr) {
        m_sceneHistory.push_back(m_scene);
        m_scene->onHide();
    }

    m_scene = scene;
    m_scene->onPush();
}

void Game::popScene() {
    m_scene->onPop();

    if(!m_sceneHistory.empty()) {
        m_scene = m_sceneHistory.back();
        m_sceneHistory.pop_back();
    } else {
        m_scene = std::make_shared<MainMenuScene>();
    }

    m_scene->onPush();
}

void Game::checkSceneFlags(std::shared_ptr<Container> scene) {
    for(auto& child : scene->getChildren()) {
        if(child->getFlag(FLAG_ALWAYS_CENTER)) {
            child->setPos(scene->getSize() / 2);
        }

        if(child->getFlag(FLAG_GUI_SCALE)) {
            child->setScale(m_guiScale);
        }

        auto container = std::dynamic_pointer_cast<Container>(child);
        if(container != nullptr) {
            checkSceneFlags(container);
        }
    }
}

void Game::init(std::vector<std::string>& args) {
    m_window.Init(1280, 720, "SFF Sandbox", FLAG_WINDOW_RESIZABLE);
    m_window.SetMinSize(640, 360);
    m_audioDevice.emplace();
    raylib::Window::SetExitKey(-1);

    m_lastWindowSize = Vector2 {1280, 720};
    m_close = false;

    auto home = Platform::getHomeDir();

    CHECK_DIR(home);
    CHECK_DIR(home / "worlds");

    // Load assets
    auto& resources = ResourceManager::Get();
    auto stm = SettingsManager::get();

    resources.LoadTexture("player.png");
    resources.LoadTexture("sff.png");
    resources.LoadTexture("raylib.png");
    resources.LoadTilemap("tiles.png", 16, 16);
    resources.LoadTilemap("items.png", 16, 16);
    resources.LoadTilemap("gui.png", 8, 8);
    resources.LoadTilemap("developers.png", 6, 1);
    resources.LoadTilemap("indicators.png", 6, 1);
    resources.LoadFont("fonts/boldfont.fnt");
    resources.LoadFont("fonts/font.fnt");
    resources.LoadMusic("menu.mp3");

    m_guiScale = getMaximumGuiScale();

#ifdef PLATFORM_ANDROID
    m_controlType = CONTROL_TOUCH;
#else
    m_controlType = CONTROL_KEYBOARD_MOUSE;
#endif

    // Load settings
    SettingsManager::get();
    StyleManager::get();
    RecipesManager::get();
    // Debug::get()->allocateConsole();
    Tile::init();

    // Load main menu
    pushScene(std::make_shared<MainMenuScene>());
    resources.GetMusic("menu.mp3").Play().SetLooping(true);

    while (!m_close) {
        if(raylib::Window::ShouldClose()) {
            m_close = true;
        }

        update();
        render();
    }

    destroy();
}

void Game::update() {
    ResourceManager::Get().UpdateMusic();

    if(m_scene != nullptr) {
        auto debug = Debug::get();

        checkSceneFlags(m_scene);

        if(raylib::Keyboard::IsKeyPressed(KEY_BACK) || raylib::Keyboard::IsKeyPressed(KEY_ESCAPE) && m_scene->isKeyBackEnabled()) {
            m_scene->keyBackClicked();
        }

        if(raylib::Keyboard::IsKeyPressed(KEY_F3)) {
            debug->toggleVisibility();
        }

        if(m_scene->shouldDestroy()) {
            popScene();
        }

        m_scene->update();
        // debug->update();
    }

    if(raylib::Keyboard::GetKeyPressed() > 0 || raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON) || raylib::Mouse::IsButtonPressed(MOUSE_RIGHT_BUTTON)) {
        m_controlType = CONTROL_KEYBOARD_MOUSE;
    }

    if(raylib::Touch::GetPointCount() > 0) {
        m_controlType = CONTROL_TOUCH;
    }

    if(raylib::Gamepad::GetButtonPressed() > 0) {
        m_controlType = CONTROL_GAMEPAD;
    }

// #ifndef PLATFORM_ANDROID
//     if(raylib::Window::IsResized()) {
//         m_lastWindowSize = {raylib::Window::GetWidth(), raylib::Window::GetHeight()};
//     }
// #endif
}

void Game::render() {
    m_window.BeginDrawing();
        m_window.ClearBackground((m_scene ? m_scene->getColor() : raylib::Color::White()));

        if(m_scene != nullptr) {
            m_scene->draw();
        }
    m_window.EndDrawing();
}

void Game::destroy() {
    auto mp = Multiplayer::get();

    if (mp->isConnected()) {
        mp->destroy();
    }

    m_sceneHistory.clear();
    m_scene.reset();
    m_player.reset();
    m_world.reset();
    MenuBase::releaseBackground();
    ResourceManager::Get().Clear();
    m_audioDevice.reset();
    m_window.Close();
}

int Game::getMaximumGuiScale() {
    return std::min(raylib::Window::GetWidth() / 640, raylib::Window::GetHeight() / 360);
}

void Game::updateGuiScale() {
    auto maxScale = getMaximumGuiScale();

    if(m_guiScale > maxScale) {
        m_guiScale = maxScale;
    }
}