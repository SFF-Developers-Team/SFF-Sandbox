#include <ui/MultiplayerScene.hpp>
#include <ui/JoinServerScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <Game.hpp>

MultiplayerScene::MultiplayerScene() : MenuBase() {
    m_username.resize(32);
    m_hostname.resize(256);
}

MultiplayerScene::~MultiplayerScene() {}

void MultiplayerScene::draw() {
    MenuBase::draw();

    auto game = Game::get();
    auto sw = static_cast<float>(GetScreenWidth());
    auto sh = static_cast<float>(GetScreenHeight());

    auto const inputW = 200.f;

    drawInput(m_username, {sw / 2.f - inputW / 2.f, 300.f, inputW, 40.f}, "Nickname:", 22.f);
    drawInput(m_hostname, {sw / 2.f - inputW / 2.f, 380.f, inputW, 40.f}, "IP:", 22.f);

    drawButtonsV({sw / 2.f - inputW / 2.f, 460.f}, {200, 40}, 60.f, {
        {"Join", [&]() {
            if(m_username.length() < 3 || m_hostname.empty()) {
                return;
            }

            game->setUsername(m_username);

            auto colon = m_hostname.find(':');
            std::string ip = m_hostname.substr(0, colon);
            uint16_t port = (colon != std::string::npos ? std::stoi(m_hostname.substr(colon + 1)) : 7777);
            
            game->pushScene(std::make_shared<JoinServerScene>(ip, port)); 
        }},
        {"Back", [game]() { 
            game->popScene(); 
        }}
    });
}