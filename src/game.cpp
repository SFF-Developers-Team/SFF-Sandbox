#include <game.hpp>
#include <Sprite.hpp>
#include <Debug.hpp>

void Game::init() {
    InitWindow(m_screenWidth, m_screenHeight, "FriendsTeam Sandbox");
    SetTargetFPS(60);

    Sprite::loadSprites();

    m_world = new World(256, 64);
    m_player = new Player();

    m_player->camera.offset = {m_screenWidth / 2.0f, m_screenHeight / 2.0f};

    while (!WindowShouldClose()) {
        this->update();
        this->render();
    }

    CloseWindow();
}
    
void Game::render() {
    BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode2D(this->m_player->camera);
            m_world->draw(m_debug);
            m_player->draw();
        EndMode2D();

        if(Debug::m_debug){
            Debug::draw();
        }
    EndDrawing();
}

void Game::update() {
    if(IsKeyPressed(KEY_F3)) {
        Debug::m_debug = !Debug::m_debug;
    }

    Debug::addString(TextFormat("FPS: %d", GetFPS()));

    m_world->update(m_player->getPosition(), m_renderDistance);
    m_player->update(m_world->m_hitboxes);
}