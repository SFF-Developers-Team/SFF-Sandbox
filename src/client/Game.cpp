#include <TextureManager.hpp>
#include <SoundManager.hpp>
#include <entity/Player.hpp>
#include <ui/MenuScene.hpp>
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
    {TEXT_SIZE, 20},
    {BORDER_WIDTH, 5}
};

void Game::pushScene(std::shared_ptr<Scene> scene) {
    m_scene = scene;
}

void Game::init(std::vector<std::string>& args) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "SFF Sandbox");
    InitAudioDevice();

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));


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
    
    sm->loadMusic("assets/menu.mp3");
    
    m_blocksMap = std::make_shared<TileMap>("assets/blocks.png", Vector2 {16, 16});
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);

    m_username = (args.size() > 0 ? args[0] : std::string("Player").append(std::to_string(rand())));
    m_isMultiplayer = args.size() > 1;

    // Load styles
    for(auto& [prop, val] : style) {
        GuiSetStyle(DEFAULT, prop, val);
    }
    
    pushScene(std::make_shared<MenuScene>());

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