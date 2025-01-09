#include <string>
#include <chrono>
#include <memory>
#include "Game.hpp"
#include <ui/MenuScene.hpp>
#include <Debug.hpp>
#include <Game.hpp>
#include <GamePacket.hpp>
#include <GitHash.hpp>
#include <Logger.hpp>
#include <TextureManager.hpp>
#include <world/gen/WorldGenNormal.hpp>
#include <chrono>
#include <entity/Player.hpp>
#include <filesystem>
void Game::menuPreRender() {
    auto tex = TextureManager::get()->getTexture("player.png");

    BeginTextureMode(m_bgTex);
        ClearBackground(SKYBLUE);
        for(int i = 0; i < m_bgArr.size(); i++) {
            if(m_bgArr[i] == 4) continue;
            m_blocksMap->drawTilePro(m_bgArr[i], {(float)(i % m_bgwidth) * 16, (float)(i / m_bgwidth) * 16, 16, 16}, RAYWHITE);
        }
        DrawTexturePro(tex, {(float)tex.width / 17 * 15, 0, (float) tex.width / 17 * -1, (float)tex.height}, {70, 14, (float)tex.width / 17, (float)tex.height}, {0, 0}, 0, WHITE);
    EndTextureMode();
}
#include <raygui.h>

void Game::setRayGuiStyle() {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSetStyle(0, 0, 0x292B56ff);
    GuiSetStyle(0, 1, 0x1a1c47ff);  
    GuiSetStyle(0, 2, 0xd8d8d8ff);
    GuiSetStyle(0, 3, 0x383A65ff);
    GuiSetStyle(0, 4, 0x292B56ff);
    GuiSetStyle(0, 5, 0xd8d8d8ff);
    GuiSetStyle(0, 6, 0x464770ff);
    GuiSetStyle(0, 7, 0x292B56ff);
    GuiSetStyle(0, 8, 0xd8d8d8ff);
    GuiSetStyle(0, BORDER_WIDTH, 5);
}

void Game::pushScene(std::shared_ptr<Scene> scene) {
    m_scene = scene;
}
void Game::init(std::vector<std::string>& args) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(m_screenWidth, m_screenHeight, "SFF Sandbox");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    SetWindowSize(m_screenWidth, m_screenHeight);

    if (enet_initialize() != 0) {
        logE("An error occurred while initializing ENet.");
    }
    InitAudioDevice();

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

    m_bgTex = LoadRenderTexture(m_bgwidth * 16, m_bgheight * 16);
    m_timer = std::make_shared<Timer>(60);
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);

    m_username = (args.size() > 0 ? args[0] : std::string("Player").append(std::to_string(rand())));
    m_isMultiplayer = args.size() > 1;

    setRayGuiStyle();
    menuPreRender();
    PlayMusicStream(sm->getMusic("menu.mp3"));
    if(m_isMultiplayer) {
        logD("Starting multiplayer session...");    
        std::string address = args[1];
        uint16_t port = 7777;
        auto delimeter = address.find_first_of(':');
        if (delimeter != std::string::npos) {
            delimeter++;
            port = std::stoi(address.substr(delimeter, address.size() - delimeter));
        }

        SetWindowTitle(std::format("SFF Sandbox ({}) - {}:{}", m_username, address, port).c_str());
        m_isMultiplayer = Multiplayer::get()->connect(address, port);

        // Временно 
        if (!m_isMultiplayer)
            std::exit(1);
    }

    if (!m_isMultiplayer) {
        if (!m_world->load()) {
            m_world->setGenerator(std::make_shared<WorldGenNormal>(m_world, 1));
            m_world->generate();
        }

        m_world->addPlayer(1, m_player);
    }
    
    Game::get()->pushScene(std::make_shared<MenuScene>());

    Debug::get()->addString(DebugID::TIME_SPENT, "Time spent in world: {}", m_world->getSpentTime());

    while (!WindowShouldClose()) {
        this->updateMenu();
        this->renderMenu();
    }

    if (!m_isMultiplayer && !IsKeyPressed(KEY_F1)) {
        m_world->save();
    }

    CloseWindow();
    CloseAudioDevice();
}

void Game::drawCrosshair(Vector2 pos) {
    const float thickness = 3.0f;
    const float size = 20.f;
    DrawLineEx({pos.x - thickness / 2.f, pos.y - size / 2.f + thickness / 2.f}, {pos.x - thickness / 2.f, pos.y + size / 2.f - thickness / 2.f}, thickness, WHITE);
    DrawLineEx({pos.x - size / 2.f + thickness / 2.f, pos.y - thickness / 2.f}, {pos.x + size / 2.f - thickness / 2.f, pos.y - thickness / 2.f}, thickness, WHITE);
}

void Game::updateMenu() {
    auto sm = SoundManager::get();

    if(GetMusicTimePlayed(sm->getMusic("menu.mp3")) >= GetMusicTimeLength(sm->getMusic("menu.mp3"))) {
        StopMusicStream(sm->getMusic("menu.mp3"));
        PlayMusicStream(sm->getMusic("menu.mp3"));
    }
    if(m_scene != nullptr) m_scene->update();
}
void Game::renderMenu() {
    auto tm = TextureManager::get();
    double delta = GetFrameTime();
    double m_timeTest;
    m_timeTest += delta;
    float pos = 100.f + (sinf(m_timeTest) * 30);

    BeginDrawing();
        ClearBackground((m_scene ? m_scene->getColor() : WHITE));   
        if(!m_inPlayScene) {
            DrawTexturePro(m_bgTex.texture, {0, 0, (float)m_bgTex.texture.width, (float)-m_bgTex.texture.height}, {-47, 235, 885, 485}, {0, 0}, 0, WHITE);
            DrawTexture(tm->getTexture("sff.png"), (GetScreenWidth() - tm->getTexture("sff.png").width) / 2, pos, WHITE);
            DrawText("Sandbox", (GetScreenWidth() - 35) / 2 - (tm->getTexture("sff.png").width - 35) / 2 - 5, pos + 35 + tm->getTexture("sff.png").width / 2, 35, RAYWHITE);
        }
        if(m_scene != nullptr) m_scene->draw();
    EndDrawing()
}

void Game::update() {
    if(m_scene != nullptr) { 
        m_scene->update();
    }
}