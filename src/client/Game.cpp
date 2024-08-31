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

void Game::init(std::vector<std::string>& args) {
#ifdef _WIN32
    setlocale(LOCALE_ALL, "ru");
    SetConsoleOutputCP(CP_UTF8);
#endif
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(m_screenWidth, m_screenHeight, "SFF Sandbox");
    SetTargetFPS(60);


    sockpp::initialize();

    m_username = (args.size() > 0 ? args[0] : std::string("Player").append(std::to_string(rand()))); 
    m_multiplayer = args.size() > 1;
    m_blocksMap = new TileMap("assets/blocks.png", Vector2 {16, 16});
    m_timer = new Timer(60);
    m_world = new World(256, 128);
    m_player = std::make_shared<Player>(m_world);
    // m_particleManager = new ParticleManager(m_world, m_player);
    m_renderManager = new RenderManager(m_world, m_player);
    m_multiplayerManager = new Multiplayer();

    if(!m_multiplayer || !m_multiplayerManager->connect(args[1], (args.size() > 2 ? atoi(args[2].c_str()) : 7777))) {
        if(!m_world->load()) {
            m_world->generate(std::make_shared<WorldGenNormal>(m_world, 1));
        }

        m_world->addPlayer(1, m_player);
    }

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    if(!m_multiplayer) {
        m_world->save();
    }

    CloseWindow();
}

Game::~Game() {
    delete m_world;
    delete m_timer;
    delete m_blocksMap;
    delete m_particleManager;
    delete m_renderManager;
    delete m_multiplayerManager;
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
            // m_particleManager->render();
        EndMode2D();

        // Selected block
        m_blocksMap->drawTilePro((uint16_t)m_player->getSelectedBlock() - 1, {m_screenWidth - 42.f, 10.f, 32.f, 32.f}, WHITE);

        if(Debug::m_debug){
            Debug::draw();
        }

        DrawText(std::format("SFF Sandbox {}-dev", GitHash::shortSha1).c_str(), 5, 5, 20, WHITE);
        DrawText(std::format("{} FPS", GetFPS()).c_str(), 5, 30, 20, WHITE);

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
        Debug::m_debug = !Debug::m_debug;
    }

    if(IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    if(m_multiplayer) {
        m_multiplayerManager->onTick();
    }
}