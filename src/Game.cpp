#include <Game.hpp>
#include <Debug.hpp>
#include <string>
#include <Player.hpp>
#include <WorldGenNormal.hpp>


void Game::init() {
    InitWindow(m_screenWidth, m_screenHeight, "SFF Sandbox");
    SetTargetFPS(60);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    
    m_blocksMap = new TileMap("assets/blocks.png", Vector2 {16, 16});
    m_world = new World(256, 128);
    m_particleManager = new ParticleManager(m_world);

    if(!m_world->load()){
        m_world->generate(new WorldGenNormal(m_world));
    }

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    m_world->save();

    CloseWindow();
}

Game::~Game() {
    delete m_world;
    delete m_blocksMap;
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

        BeginMode2D(this->m_world->getPlayer()->getCamera());
            m_world->draw();
            m_particleManager->render();
        EndMode2D();

        // Selected block
        m_blocksMap->drawTilePro((uint16_t)m_world->getPlayer()->getSelectedBlock() - 1, {m_screenWidth - 42.f, 10.f, 32.f, 32.f}, WHITE);

        if(Debug::m_debug){
            Debug::draw();
        }

        // auto cur = GetMousePosition();
        // drawCrosshair(cur);
    EndDrawing();
}

void Game::update() {
    if(IsKeyPressed(KEY_F3)) {
        Debug::m_debug = !Debug::m_debug;
    }

    if(IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    std::string fps = "FPS: " + std::to_string(GetFPS());
    Debug::addString(fps);

    m_world->update();
    m_particleManager->update();
}
