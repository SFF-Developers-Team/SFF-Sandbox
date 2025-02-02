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

    m_state = std::make_shared<Text>("font", "", 32.f);
    m_state->setPos({screenW / 2, 300.f});
    m_state->setSize({screenW, 30.f});
    m_state->setTag("state");
    addChild(m_state);
}

void JoinServerScene::update() {
    MenuBase::update();

    auto game = Game::get();
    auto mp = Multiplayer::get();

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
    
        if(!m_cancelBtn) {
            m_cancelBtn = std::make_shared<Button>("Cancel", [&] (Button*) {
                mp->destroy();
            });
            m_cancelBtn->setPos({GetScreenWidth() / 2.f, 460.f});
            m_cancelBtn->setSize({200.f, 40.f});

            addChild(m_cancelBtn);
        }
    }

    m_state->setText(m_message);
}