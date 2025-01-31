#include <ui/MultiplayerScene.hpp>
#include <ui/JoinServerScene.hpp>
#include <ui/nodes/Text.hpp>
#include <ui/nodes/Button.hpp>
#include <ui/ErrorScene.hpp>
#include <ui/PlayScene.hpp>
#include <Multiplayer.hpp>
#include <Game.hpp>
#include <format>
#include <thread>

JoinServerScene::JoinServerScene(std::string const& hostname, uint16_t port) : m_startTime(GetTime()) {
    auto mp = Multiplayer::get();
    std::thread(&Multiplayer::connect, mp, hostname, port).detach();

    auto const screenW = static_cast<float>(GetScreenWidth());
    auto const screenH = static_cast<float>(GetScreenHeight());
    auto const inputW = 200.f;

    auto state = std::make_shared<Text>("font", "", 32.f);
    state->setPos({screenW / 2, 300.f});
    state->setSize({screenW, 30.f});
    state->setTag("state");
    addChild(state);
}

void JoinServerScene::update() {
    auto game = Game::get();
    auto mp = Multiplayer::get();
    auto state = getChild<Text>("state");

    switch(mp->getState()) {
        case CONNECTING:
            m_message = "Connecting to server";
            break;       
        case LOGGING_IN:
            mp->update();
            m_message = "Logging in";
            break;
        case ERROR:
            game->clearSceneHistory();
            game->pushScene(std::make_shared<ErrorScene>(mp->getError()));
            return;
            
        case PLAYING:
            game->clearSceneHistory();
            game->pushScene(std::make_shared<PlayScene>(true));
            break;
    }

    if(GetTime() > m_dotTime + 1.f) {
        m_dotTime = GetTime();
        m_dots++;
    }

    if(m_dots > 3) m_dots = 0;

    for(int i = 0; i < m_dots; i++) m_message += '.';

    if(GetTime() >= m_startTime + 5.f) {
        m_message += std::format(" ({}s)", static_cast<int>(GetTime() - m_startTime));
    }

    state->setText(m_message);
}

void JoinServerScene::draw() {
    MenuBase::draw();

    // drawText(m_message, {screenW / 2.f, screenH / 2.f}, 28.f, true);

    // if(GetTime() >= m_startTime + 5.f) {
    //     drawButton("Cancel", {screenW / 2.f - inputW / 2.f, 460.f, 200.f, 40.f}, []() -> void {
    //         Multiplayer::get()->destroy();
    //         Game::get()->popScene();
    //     });
    // }
}