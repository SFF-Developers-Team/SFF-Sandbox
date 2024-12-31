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
#include <ui/CallbackNode.hpp>
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

    SetWindowSize(m_screenWidth * sandbox_ui::Node::getDpiScaling(), m_screenHeight * sandbox_ui::Node::getDpiScaling());
  
    sockpp::initialize();

    m_username = (args.size() > 0 ? args[0] : std::string("Player").append(std::to_string(rand()))); 
    m_multiplayer = args.size() > 1;
    m_blocksMap = std::make_shared<TileMap>("assets/blocks.png", Vector2 {16, 16});
    m_timer = std::make_shared<Timer>(60);
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);
    // m_particleManager = std::make_shared<ParticleManager>(m_world, m_player);
    m_renderManager = std::make_shared<RenderManager>(m_world, m_player);
    m_multiplayerManager = std::make_shared<Multiplayer>();
    m_gameMenu = std::make_shared<sandbox_ui::InitialMenu>();
    m_uiRenderer = std::make_shared<sandbox_ui::NodeRenderer>();

    auto tm = TextureManager::get();
    tm->loadTexture(std::filesystem::path("assets/player.png"));

    m_uiRenderer->setScaling(8);
    m_uiRenderer->addChild(m_gameMenu);

    m_player->disableInput(true);
    m_player->unlinkCameraX(true);

    auto cn = std::make_shared<sandbox_ui::CallbackNode>();
    cn->setDrawCallback([this](auto node) {
        auto cam = m_player->getCamera();
        auto old_cam = cam;

        m_gameMenu->m_worldCam.y = cam.target.y + 4;
        m_gameMenu->m_worldCam.x += 0.001f;

        auto container = m_gameMenu->getNodeContainer();
        float mscale = container->getScaling();

        container->setScaling(1.f / 5.f);

        auto m = m_gameMenu->getNodeContainer()->getMappedPosition(m_gameMenu->m_worldCam);

        cam.target.x = m.x;
        cam.target.y = m_gameMenu->m_worldCam.y;

        // printf("cam.target.x=%f\n", m.x);

        m_player->setCamera(cam);

        BeginMode2D(cam);

        m_renderManager->renderWorld();

        EndMode2D();

        m_player->setCamera(old_cam);
        container->setScaling(mscale);
    });
    cn->setSize((float)m_screenWidth, (float)m_screenHeight);
    cn->setID("game-viewpoint");

    m_gameMenu->getNodeContainer()->addChild(cn, -1);

    if(!m_multiplayer || !m_multiplayerManager->connect(args[1], (args.size() > 2 ? atoi(args[2].c_str()) : 7777))) {
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

    if(!m_multiplayer) {
        // m_world->save();
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

        if(m_inPlayScene) {
            BeginMode2D(m_player->getCamera());
                m_renderManager->renderWorld();
            EndMode2D();
            auto selectedBlock = m_player->getSelectedBlock();
            if(selectedBlock) {
                m_renderManager->renderSelectedBlock(m_screenWidth - 42.f, 10.f, selectedBlock);
            }
        } else {
            m_uiRenderer->render();
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

    if(m_multiplayer) {
        m_multiplayerManager->onTick();
    }
}