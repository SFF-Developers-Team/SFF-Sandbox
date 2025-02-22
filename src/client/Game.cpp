#include <TextureManager.hpp>
#include <SoundManager.hpp>
#include <entity/Player.hpp>
#include <ui/MainMenuScene.hpp>
#include <world/World.hpp>
#include <TileMap.hpp>
#include <Logger.hpp>
#include <Game.hpp>
#include <SettingsManager.hpp>
#include <StyleManager.hpp>

void Game::pushScene(std::shared_ptr<Scene> scene) {
    if(m_scene != nullptr) {    
        m_sceneHistory.push_back(m_scene);
        m_scene->onHide();
    }

    m_scene = scene;
    m_scene->onShow();
}

void Game::popScene() {
    if(!m_sceneHistory.empty()) {
        m_scene = m_sceneHistory.back();
        m_sceneHistory.pop_back();
        m_scene->onShow();
    }
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
    InitWindow(GetScreenWidth(), GetScreenHeight(), "SFF Sandbox");
#ifndef PLATFORM_ANDROID
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_FULLSCREEN_MODE);
#endif
    InitAudioDevice();
    SetExitKey(-1);

    // Load assets
    auto sm = SoundManager::get();
    auto tm = TextureManager::get();
    auto stm = SettingsManager::get();

    tm->loadTexture("player.png");
    tm->loadTexture("sff.png");
    tm->loadTexture("raylib.png");
    tm->loadTileMap("blocks.png", {16, 16});
    tm->loadTileMap("gui.png", {16, 16});
    tm->loadTileMap("developers.png", {128, 128});
    tm->loadFont("fonts/boldfont.fnt");
    tm->loadFont("fonts/font.fnt");
    sm->loadMusic("menu.mp3");
    
    // Load main classes
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);

    m_lastWindowSize = {1280, 720};
    m_guiScale = getMaximumGuiScale();
    m_close = false;

#ifdef PLATFORM_ANDROID
    m_controlType = CONTROL_TOUCH;
#else
    m_controlType = CONTROL_KEYBOARD_MOUSE;
#endif
    
    // Load settings
    SettingsManager::get();
    StyleManager::get();
    
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

    if(GetKeyPressed() > 0 || IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        m_controlType = CONTROL_KEYBOARD_MOUSE;
    }

    if(GetTouchPointCount() > 0) {
        m_controlType = CONTROL_TOUCH;
    }

    if(GetGamepadButtonPressed() > 0) {
        m_controlType = CONTROL_GAMEPAD;
    }

    if(m_scene != nullptr) {
        checkSceneFlags(m_scene);
        m_scene->update();

        if(IsKeyPressed(KEY_BACK)) {
            m_scene->keyBackClicked();
        }

        if(m_scene->shouldDestroy()) {
            popScene();
        }
    }

#ifndef PLATFORM_ANDROID
    if(IsWindowResized()) {
        m_lastWindowSize = {GetScreenWidth(), GetScreenHeight()};
    }
#endif
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
    CloseAudioDevice();
    CloseWindow();
}

int Game::getMaximumGuiScale() { 
    return std::min(GetScreenWidth() / 640, GetScreenHeight() / 360); 
}