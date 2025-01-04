#include <string>
#include <chrono>
#include <memory>
#include "Game.hpp"
#include <Debug.hpp>
#include <Player.hpp>
#include <Logger.hpp>
#include <Chunk.hpp>
#include <WorldGenNormal.hpp>
#include <GamePacket.hpp>
#include <GitHash.hpp>
#include <TextureManager.hpp>
#include <filesystem>

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

    m_blocksMap = std::make_shared<TileMap>("assets/blocks.png", Vector2 {16, 16});
    m_timer = std::make_shared<Timer>(60);
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);
    // m_particleManager = std::make_shared<ParticleManager>(m_world, m_player);
    m_renderManager = std::make_shared<RenderManager>(m_world, m_player);

    auto tm = TextureManager::get();
    tm->loadTexture(std::filesystem::path("assets/player.png"));

    m_username = (args.size() > 0 ? args[0] : std::string("Player").append(std::to_string(rand()))); 
    m_isMultiplayer = args.size() > 1;

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
    
    if(!m_isMultiplayer) {
        if(!m_world->load()) {
            m_world->setGenerator(std::make_shared<WorldGenNormal>(m_world, 1));
            m_world->generate();
        }

        m_world->addPlayer(1, m_player);
    }

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    if(!m_isMultiplayer && !IsKeyPressed(KEY_F1)) {
        m_world->save();
    }

    CloseWindow();
}

void Game::drawCrosshair(Vector2 pos) {
    const float thickness = 3.0f;
    const float size = 20.f;
    DrawLineEx({pos.x - thickness / 2.f, pos.y - size / 2.f + thickness / 2.f}, {pos.x - thickness / 2.f, pos.y + size / 2.f - thickness / 2.f}, thickness, WHITE);
    DrawLineEx({pos.x - size / 2.f + thickness / 2.f, pos.y - thickness / 2.f}, {pos.x + size / 2.f - thickness / 2.f, pos.y - thickness / 2.f}, thickness, WHITE);
}
    
void Game::render() {
    BeginDrawing();
        ClearBackground(SKYBLUE);

            BeginMode2D(m_player->getCamera());
                m_renderManager->renderWorld();
            EndMode2D();

            auto selectedBlock = m_player->getSelectedBlock();
            
            if(selectedBlock) {
                m_renderManager->renderUIBlock(m_screenWidth - 42.f, 10.f, 32.f, 32.f, selectedBlock);
            }

        auto dbg = Debug::get();

        if(dbg->isVisible()){
            dbg->draw();
        } else {
            DrawText(std::format("SFF Sandbox {}-dev ({} {})", GitHash::shortSha1, __DATE__, __TIME__).c_str(), 5, 5, 20, WHITE);
            DrawText(std::format("{} FPS", GetFPS()).c_str(), 5, 30, 20, WHITE);
        }

        // auto cur = GetMousePosition();
        // drawCrosshair(cur);
    EndDrawing();
}

void Game::update() {
    m_timer->advanceTime();

    for (uint32_t i = 0; i < m_timer->getTicks(); i++) {
        m_world->onTick();
    }

    m_player->update();

    if(IsKeyPressed(KEY_F3)) {
        auto dbg = Debug::get();
        dbg->setVisible(!dbg->isVisible());
    }

    if(IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    if(IsKeyPressed(KEY_F1)) {
        CloseWindow();
    }
}