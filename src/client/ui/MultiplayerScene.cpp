#include <ui/MultiplayerScene.hpp>
#include <raygui.h>

MultiplayerScene::MultiplayerScene() {
    // m_ip = (char*)MemAlloc(64);
    // m_nickname = (char*)MemAlloc(32);
}

MultiplayerScene::~MultiplayerScene() {}

void MultiplayerScene::update() {
    Vector2 m_mousePos = GetMousePosition();

    // m_isNick = CheckCollisionPointRec(m_mousePos, m_nickBox);
    // m_isIP = CheckCollisionPointRec(m_mousePos, m_ipBox);
}

void MultiplayerScene::draw() {

    // DrawText("Nickname:", (float)(GetScreenWidth() - 200) / 2 - 125, 300, 25, RAYWHITE);
    // DrawText("IP:", (float)(GetScreenWidth() - 200) / 2 - 50, 360, 25, RAYWHITE);

    // GuiTextBox(m_nickBox, m_nickname, 32, m_isNick);
    // GuiTextBox(m_ipBox, m_ip, 64, m_isIP);

    // if(GuiButton((Rectangle){ (float)(GetScreenWidth() - 200) / 2, 420, 200, 40 }, "Join")) {
    //     // ..
    // };
    // if(GuiButton((Rectangle){ (float)(GetScreenWidth() - 200) / 2, 480, 200, 40 }, "Back") || IsKeyPressed(KEY_Q)) {
    //     auto game = Game::get();
    //     game->pushScene(std::make_shared<MainScene>());
    // }
}