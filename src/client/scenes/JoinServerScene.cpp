#include <raylib-cpp.hpp>
#include <scenes/MultiplayerScene.hpp>
#include <scenes/JoinServerScene.hpp>
#include <scenes/ErrorScene.hpp>
#include <scenes/PlayScene.hpp>
#include <ui/basic/Text.hpp>
#include <ui/basic/Button.hpp>
#include <net/Multiplayer.hpp>
#include <world/World.hpp>
#include <Game.hpp>
#include <format>
#include <thread>

JoinServerScene::JoinServerScene(std::string const& hostname, uint16_t port) : m_startTime(raylib::Window::GetTime()) {
    auto mp = Multiplayer::get();
    std::thread(&Multiplayer::connect, mp, hostname, port).detach();

    m_state = std::make_shared<Text>("font", "");
    m_state->setPos({getWidth() / 2, getHeight() / 2});
    m_state->setFlags(FLAG_GUI_SCALE | FLAG_ALWAYS_CENTER);
    m_state->setWidth(getWidth());
    m_state->setTag("state");
    m_state->setScale(Game::get()->getGuiScale());
    addChild(m_state);
}

void JoinServerScene::update() {
    MenuBase::update();

    auto game = Game::get();
    auto mp = Multiplayer::get();

    switch(mp->getState()) {
        case MultiplayerState::CONNECTING:
            m_message = "Connecting to server";
            break;       
        case MultiplayerState::LOGGING_IN:
            mp->update();
            m_message = "Logging in";
            break;

        case MultiplayerState::LOADING_TERRAIN:
            mp->update();
            m_message = "Loading terrain";
            break;
        case MultiplayerState::ERROR:
            game->pushScene(std::make_shared<ErrorScene>(mp->getError()));
            return;
            
        case MultiplayerState::PLAYING:
            game->pushScene(std::make_shared<PlayScene>(true));
            break;
    }

    if(raylib::Window::GetTime() > m_dotTime + 1.f) {
        m_dotTime = raylib::Window::GetTime();
        m_dots++;
    }

    if(m_dots > 3) m_dots = 0;

    for(int i = 0; i < m_dots; i++) m_message += '.';

    if(raylib::Window::GetTime() >= m_startTime + 5.f) {
        m_message += std::format(" ({}s)", static_cast<int>(raylib::Window::GetTime() - m_startTime));
    
        if(!m_cancelBtn) {
            m_cancelBtn = std::make_shared<Button>("Cancel", [this, mp] (Button*) {
                this->destroy();
                mp->destroy();
            });

            m_cancelBtn->setPos({getWidth() / 2.f, m_state->getBottomY() + 40.f});
            m_cancelBtn->setFlags(FLAG_GUI_SCALE);
            m_cancelBtn->setAnchorY(0.f);
            addChild(m_cancelBtn);
        }
    }

    m_state->setText(m_message);
}