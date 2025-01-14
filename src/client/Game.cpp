#include <TextureManager.hpp>
#include <SoundManager.hpp>
#include <entity/Player.hpp>
#include <ui/MainMenuScene.hpp>
#include <world/World.hpp>
#include <TileMap.hpp>
#include <Logger.hpp>
#include <Game.hpp>
#include <raygui.h>

std::map<int, int> const style = {
    {BORDER_COLOR_NORMAL, 0x292B56ff},
    {BASE_COLOR_NORMAL, 0x1a1c47ff},
    {TEXT_COLOR_NORMAL, 0xd8d8d8ff},

    {BORDER_COLOR_FOCUSED, 0x383A65ff},
    {BASE_COLOR_FOCUSED, 0x292B56ff},
    {TEXT_COLOR_FOCUSED, 0xd8d8d8ff},
    
    {BORDER_COLOR_PRESSED, 0x464770ff},
    {BASE_COLOR_PRESSED, 0x292B56ff},
    {TEXT_COLOR_PRESSED, 0xd8d8d8ff},

    {BACKGROUND_COLOR, 0x1a1c47ff},
    
    {TEXT_SIZE, 20},
    {BORDER_WIDTH, 5}
};

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
    // ToggleFullscreen();

    auto sm = SoundManager::get();
    auto tm = TextureManager::get();

    tm->loadTexture("assets/player.png");
    tm->loadTexture("assets/sff.png");
    tm->loadTexture("assets/kolyah35.png");
    tm->loadTexture("assets/sergeymc9730.png");
    tm->loadTexture("assets/invisedivine.png");
    tm->loadTexture("assets/e2e4.png");
    tm->loadTexture("assets/del.png");
    tm->loadTexture("assets/player.png");
    tm->loadTexture("assets/crosshair.png");
    tm->loadTexture("assets/selected.png");
    tm->loadTileMap("assets/blocks.png", {16, 16});
    tm->loadTileMap("assets/gui.png", {16, 16});

    sm->loadMusic("assets/menu.mp3");
    
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);
    PlayMusicStream(sm->getMusic("menu.mp3"));
    // Load styles
    for(auto& [prop, val] : style) {
        GuiSetStyle(DEFAULT, prop, val);
    }
    
    pushScene(std::make_shared<MainMenuScene>());

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    // if (!m_isMultiplayer && !IsKeyPressed(KEY_F1)) {
    //     m_world->save();
    // }

    CloseWindow();
    CloseAudioDevice();
}

void Game::update() {
    auto sm = SoundManager::get();
    UpdateMusicStream(sm->getMusic("menu.mp3"));
  
    if(GetMusicTimePlayed(sm->getMusic("menu.mp3")) >= GetMusicTimeLength(sm->getMusic("menu.mp3"))) {
        StopMusicStream(sm->getMusic("menu.mp3"));
        PlayMusicStream(sm->getMusic("menu.mp3"));
    }
  
    if(m_scene != nullptr) m_scene->update();
}

void Game::render() {
    BeginDrawing();
        ClearBackground((m_scene ? m_scene->getColor().to<Color>() : WHITE));   
        if(m_scene != nullptr) m_scene->draw();
    EndDrawing();
}