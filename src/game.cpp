#include <game.hpp>
#include <Sprite.hpp>
#include <player.hpp>

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

        DrawText(TextFormat("Player position %.02f %.02f", m_player->camera.target.x, m_player->camera.target.y), 0, 0, 24, WHITE);
    EndDrawing();
}

void Game::update() {
    if(IsKeyPressed(KEY_F3)) {
        m_debug = !m_debug;
    }

    m_player->update();
    m_world->update(m_player->getPosition(), m_renderDistance);
}