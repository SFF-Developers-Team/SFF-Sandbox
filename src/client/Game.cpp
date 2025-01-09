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
#include <memory>
#include <string>
#include <world/Chunk.hpp>

using namespace std::chrono;


void Game::init(std::vector<std::string>& args) {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(m_screenWidth, m_screenHeight, "SFF Sandbox");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    SetWindowSize(m_screenWidth, m_screenHeight);

    if (enet_initialize() != 0) {
        logE("An error occurred while initializing ENet.");
    }

    m_blocksMap = std::make_shared<TileMap>("assets/blocks.png", Vector2 {16, 16});
    m_timer = std::make_shared<Timer>(60);
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);

    auto tm = TextureManager::get();
    tm->loadTexture(std::filesystem::path("assets/player.png"));
    tm->loadTexture(std::filesystem::path("assets/crosshair.png"));
    tm->loadTexture(std::filesystem::path("assets/selected.png"));

    m_username = (args.size() > 0 ? args[0] : std::string("Player").append(std::to_string(rand())));
    m_isMultiplayer = args.size() > 1;

    if (m_isMultiplayer) {
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

    Debug::get()->addString(DebugID::TIME_SPENT, "Time spent in world: {}", m_world->getSpentTime());

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    if (!m_isMultiplayer && !IsKeyPressed(KEY_F1)) {
        m_world->save();
    }

    CloseWindow();
}

void Game::render() {
    auto rm = RenderManager::get();

    BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(m_player->getCamera());
            rm->renderWorld(m_world, m_player);
        EndMode2D();

        auto selectedBlock = m_player->getSelectedBlock();

        if (selectedBlock) {
            rm->renderUIBlock(m_screenWidth - 42.f, 10.f, 32.f, 32.f, selectedBlock);
        }

        Debug::get()->draw();

        auto mouse = GetMousePosition();
        rm->drawTexture("crosshair.png", {mouse.x, mouse.y, 0.5f, 0.5f}, COL_WHITE, 0.0f, {0.25f, 0.25f});
    EndDrawing();
}

void Game::update() {
    m_timer->advanceTime();

    for (uint32_t i = 0; i < m_timer->getTicks(); i++) {
        m_world->onTick();

        auto seconds = std::chrono::seconds(m_world->getSpentTime() + (std::time(NULL) - m_world->getLoadTime()));
        auto hours = duration_cast<std::chrono::hours>(seconds);
        seconds -= hours;
        auto minutes = duration_cast<std::chrono::minutes>(seconds);
        seconds -= minutes;

        Debug::get()->updateString(DebugID::TIME_SPENT, "Time spent in world: {} {} {}", hours, minutes, seconds);
    }

    m_player->update();

    if(m_isMultiplayer) {
        Multiplayer::get()->update();
    }

    if (IsKeyPressed(KEY_F3)) {
        auto dbg = Debug::get();
        dbg->setVisible(!dbg->isVisible());
    }

    if (IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    if (IsKeyPressed(KEY_F1)) {
        CloseWindow();
    }
}