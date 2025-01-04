#include <string>
#include <chrono>
#include <memory>
#include "Game.hpp"
#include <Menu.hpp>
#include <Debug.hpp>
#include <Player.hpp>
#include <Logger.hpp>
#include <Chunk.hpp>
#include <WorldGenNormal.hpp>
#include <GamePacket.hpp>
#include <GitHash.hpp>
#include <TextureManager.hpp>
#include <filesystem>
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
#ifdef _WIN32
    setlocale(LOCALE_ALL, "ru");
    SetConsoleOutputCP(CP_UTF8);
#endif
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(m_screenWidth, m_screenHeight, "SFF Sandbox");
    SetTargetFPS(GetMonitorRefreshRate(0));

    SetWindowSize(m_screenWidth, m_screenHeight);
  
    sockpp::initialize();


    auto tm = TextureManager::get();
    tm->loadTexture(std::filesystem::path("assets/player.png"));

    tm->loadTexture(std::filesystem::path("assets/sff.png"));

    tm->loadTexture(std::filesystem::path("assets/kolyah35.png"));
    tm->loadTexture(std::filesystem::path("assets/sergeymc9730.png"));
    tm->loadTexture(std::filesystem::path("assets/invisedivine.png"));
    tm->loadTexture(std::filesystem::path("assets/e2e4.png"));
    tm->loadTexture(std::filesystem::path("assets/del.png"));
    

    m_username = (args.size() > 0 ? args[0] : std::string("Player").append(std::to_string(rand()))); 
    m_isMultiplayer = args.size() > 1;
    setRayGuiStyle();
    if(m_isMultiplayer) {
        logD("Starting multiplayer session...");    
        std::string address = args[1];
        uint16_t port = 7777;
        auto delimeter = address.find_first_of(':');
        if(delimeter != std::string::npos) {
            delimeter++;
            port = std::stoi(address.substr(delimeter, address.size() - delimeter));
        }

        SetWindowTitle(std::format("SFF Sandbox ({}) - {}:{}", m_username, address, port).c_str());
        m_isMultiplayer = Multiplayer::get()->connect(address, port);
    }
    
    // if(!m_isMultiplayer) {
    //     if(!m_world->load()) {
    //         m_world->setGenerator(std::make_shared<WorldGenNormal>(m_world, 1));
    //         m_world->generate();
    //     }

    //     m_world->addPlayer(1, m_player);
    // }
    Game::get()->pushScene(std::make_shared<MainScene>());

    while (!WindowShouldClose()) {
        this->updateMenu();
        this->renderMenu();
    }

    // if(!m_isMultiplayer && !IsKeyPressed(KEY_F1)) {
    //     m_world->save();
    // }

    CloseWindow();
}

void Game::drawCrosshair(Vector2 pos) {
    const float thickness = 3.0f;
    const float size = 20.f;
    DrawLineEx({pos.x - thickness / 2.f, pos.y - size / 2.f + thickness / 2.f}, {pos.x - thickness / 2.f, pos.y + size / 2.f - thickness / 2.f}, thickness, WHITE);
    DrawLineEx({pos.x - size / 2.f + thickness / 2.f, pos.y - thickness / 2.f}, {pos.x + size / 2.f - thickness / 2.f, pos.y - thickness / 2.f}, thickness, WHITE);
}
    
void Game::render() {
        // auto cur = GetMousePosition();
        // drawCrosshair(cur);
}

void Game::update() {
}
void Game::updateMenu() {
    if(m_scene != nullptr) m_scene->update();
}
void Game::renderMenu() {
    auto tm = TextureManager::get();

    double delta = GetFrameTime();
    double m_timeTest;
    m_timeTest += delta;
    float pos = 100.f + ((float)sin(m_timeTest) * 30);

    BeginDrawing();

        ClearBackground((m_scene ? m_scene->getColor() : WHITE));   
        
        DrawTexture(tm->getTexture("sff.png"), (GetScreenWidth() - tm->getTexture("sff.png").width) / 2, pos, WHITE);
        DrawText("Sandbox", (GetScreenWidth() - 35) / 2 - (tm->getTexture("sff.png").width - 35) / 2 - 5, pos + 35 + tm->getTexture("sff.png").width / 2, 35, RAYWHITE);
        if(m_scene != nullptr) m_scene->draw();

    EndDrawing();
}