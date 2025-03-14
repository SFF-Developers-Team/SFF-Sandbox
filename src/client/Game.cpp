#include <scenes/MainMenuScene.hpp>
#include <managers/TextureManager.hpp>
#include <managers/SoundManager.hpp>
#include <entity/Player.hpp>
#include <world/World.hpp>
#include <TileMap.hpp>
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
#if 0
    SetTraceLogCallback([](int logLevel, const char *text, va_list args) {
        std::unordered_map<int, std::string> static prefixes = {
            {LOG_ALL, "\\cw"},
            {LOG_TRACE, "\\cwTRACE: "},
            {LOG_DEBUG, "\\cwDEBUG: "},
            {LOG_INFO, "\\cwINFO: "},
            {LOG_WARNING, "\\cyWARNING: "},
            {LOG_ERROR, "\\crERROR: "},
            {LOG_FATAL, "\\crFATAL: "}
        };
        
        va_list argsCopy;
        va_copy(argsCopy, args);
        int size = vsnprintf(nullptr, 0, text, argsCopy);
        va_end(argsCopy);

        std::string buffer = prefixes[logLevel];
        buffer.resize(prefixes[logLevel].size() + size - 1);

        std::vsnprintf(buffer.data() + prefixes[logLevel].size(), size, text, args);
        buffer.append("\n");

        Debug::get()->log(buffer);
    });
#endif

    InitWindow(GetScreenWidth(), GetScreenHeight(), "SFF Sandbox");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetWindowMinSize(640, 360);
    InitAudioDevice();
    SetExitKey(-1);

    m_lastWindowSize = {1280, 720};
    m_close = false;

    auto home = Platform::getHomeDir();

    CHECK_DIR(home);
    CHECK_DIR(home / "worlds");

    // Load assets
    auto sm = SoundManager::get();
    auto tm = TextureManager::get();
    auto stm = SettingsManager::get();

    tm->loadTexture("player.png");
    tm->loadTexture("sff.png");
    tm->loadTexture("raylib.png");
    tm->loadTileMap("blocks.png", {16, 16});
    tm->loadTileMap("items.png", {16, 16});
    tm->loadTileMap("gui.png", {16, 16});
    tm->loadTileMap("developers.png", {128, 128});
    tm->loadTileMap("indicators.png", {8, 7});
    tm->loadFont("fonts/boldfont.fnt");
    tm->loadFont("fonts/font.fnt");
    sm->loadMusic("menu.mp3");
    
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

    // Load main menu
    pushScene(std::make_shared<MainMenuScene>());
    PlayMusicStream(sm->getMusic("menu.mp3"));

    while (!m_close) {
        if(WindowShouldClose()) {
            m_close = true;
        }

        update();
        render();
    }

    destroy();
}

void Game::update() {
    auto sm = SoundManager::get();
    
    UpdateMusicStream(sm->getMusic("menu.mp3"));
    if(GetMusicTimePlayed(sm->getMusic("menu.mp3")) >= GetMusicTimeLength(sm->getMusic("menu.mp3"))) {
        StopMusicStream(sm->getMusic("menu.mp3"));
        PlayMusicStream(sm->getMusic("menu.mp3"));
    }

    if(m_scene != nullptr) {
        auto debug = Debug::get();

        checkSceneFlags(m_scene);

        if(IsKeyPressed(KEY_BACK) || IsKeyPressed(KEY_ESCAPE) && m_scene->isKeyBackEnabled()) {
            m_scene->keyBackClicked();
        }

        if(IsKeyPressed(KEY_F3)) {
            debug->toggleVisibility();
        }

        if(m_scene->shouldDestroy()) {
            popScene();
        }

        m_scene->update();
        // debug->update();
    }

    if(GetKeyPressed() > 0 || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        m_controlType = CONTROL_KEYBOARD_MOUSE;
    }

    if(GetTouchPointCount() > 0) {
        m_controlType = CONTROL_TOUCH;
    }

    if(GetGamepadButtonPressed() > 0) {
        m_controlType = CONTROL_GAMEPAD;
    }

// #ifndef PLATFORM_ANDROID
//     if(IsWindowResized()) {
//         m_lastWindowSize = {GetScreenWidth(), GetScreenHeight()};
//     }
// #endif
}

void Game::render() {
    BeginDrawing();
        ClearBackground((m_scene ? m_scene->getColor().to<Color>() : WHITE));
        
        if(m_scene != nullptr) {
            m_scene->draw();
        }
    EndDrawing();
}

void Game::destroy() {
    auto mp = Multiplayer::get();

    if (mp->isConnected()) {
        mp->destroy();
    }

    CloseAudioDevice();
    CloseWindow();
}

int Game::getMaximumGuiScale() { 
    return std::min(GetScreenWidth() / 640, GetScreenHeight() / 360); 
}

void Game::updateGuiScale() {
    auto maxScale = getMaximumGuiScale();

    if(m_guiScale > maxScale) {
        m_guiScale = maxScale;
    }
}