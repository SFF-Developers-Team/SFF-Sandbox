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

void Game::clearSceneHistory() {
    m_sceneHistory.clear();
}

void Game::pushScene(std::shared_ptr<Scene> scene) {
    if(m_scene != nullptr) {
        m_sceneHistory.push_back(m_scene);
    }

    m_scene = scene;
}

void Game::popScene() {
    if(!m_sceneHistory.empty()) {
        m_scene = m_sceneHistory.back();
        m_sceneHistory.pop_back();
    }
}

void Game::init(std::vector<std::string>& args) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "SFF Sandbox");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    InitAudioDevice();

    SetExitKey(-1);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // Load assets
    auto sm = SoundManager::get();
    auto tm = TextureManager::get();

    tm->loadTexture("assets/player.png");
    tm->loadTexture("assets/sff.png");
    tm->loadTexture("assets/player.png");
    tm->loadTexture("assets/crosshair.png");
    tm->loadTexture("assets/selected.png");
    tm->loadTexture("assets/raylib.png");
    tm->loadTileMap("assets/blocks.png", {16, 16});
    tm->loadTileMap("assets/gui.png", {16, 16});
    tm->loadTileMap("assets/developers.png", {128, 128});
    tm->loadFont("assets/boldfont.fnt");
    tm->loadFont("assets/font.fnt");
    sm->loadMusic("assets/menu.mp3");
    
    // Load main classes
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);

    m_lastWindowSize = {1280, 720};
    
    // Load settings
    SettingsManager::get();
    StyleManager::get();
    
    // Load main menu
    pushScene(std::make_shared<MainMenuScene>());
    PlayMusicStream(sm->getMusic("menu.mp3"));

    while (!WindowShouldClose()) {
        update();
        render();
    }

    destroy();

    // if (!m_isMultiplayer && !IsKeyPressed(KEY_F1)) {
    //     m_world->save();
    // }
}

void Game::update() {
    auto sm = SoundManager::get();
    UpdateMusicStream(sm->getMusic("menu.mp3"));
    if(GetMusicTimePlayed(sm->getMusic("menu.mp3")) >= GetMusicTimeLength(sm->getMusic("menu.mp3"))) {
        StopMusicStream(sm->getMusic("menu.mp3"));
        PlayMusicStream(sm->getMusic("menu.mp3"));
    }

    if(m_scene != nullptr) {
        m_scene->update();

        if(m_scene->shouldDestroy()) {
            m_scene = m_sceneHistory.back();
            m_sceneHistory.pop_back();
        }
    }

    if(IsWindowResized()) {
        m_lastWindowSize = {GetScreenWidth(), GetScreenHeight()};
    }
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