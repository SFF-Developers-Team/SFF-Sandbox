#include <world/gen/WorldGenNormal.hpp>
#include <RenderManager.hpp>
#include <entity/Player.hpp>
#include <ui/PlayScene.hpp>
#include <ui/ErrorScene.hpp>
#include <world/World.hpp>
#include <Multiplayer.hpp>
#include <Types.hpp>
#include <Debug.hpp>
#include <Timer.hpp>
#include <Game.hpp>

#include <chrono>

PlayScene::PlayScene(bool isOnline) : 
    m_timer(std::make_shared<Timer>(60)), m_online(isOnline) 
{
    auto game = Game::get();
    m_world = game->getWorld();
    m_player = game->getPlayer();
    m_bgColor = COL_SKYBLUE;

    if(!m_online) {
        if (!m_world->load()) {
            m_world->setGenerator(std::make_shared<WorldGenNormal>(m_world, 1));
            m_world->generate();
        }

        m_world->addPlayer(1, m_player);
    }

    HideCursor();
}

PlayScene::~PlayScene() {
    if(!m_online) {
        m_world->save();
    }
}

void PlayScene::draw() {
    auto rm = RenderManager::get();

    BeginMode2D(m_player->getCamera());
        rm->renderWorld(m_world, m_player);
    EndMode2D();

    auto selectedBlock = m_player->getSelectedBlock();

    if (selectedBlock) {
        rm->renderBlock({GetScreenWidth() - 42.f, 10.f, 32.f, 32.f}, selectedBlock);
    }

    Debug::get()->draw();

    auto mouse = GetMousePosition();
    rm->drawTile("gui.png", 0, {mouse.x, mouse.y, 16.f, 16.f}, COL_WHITE, 0.f, {8.f, 8.f});
}

void PlayScene::update() {
    m_timer->advanceTime();

    for (uint32_t i = 0; i < m_timer->getTicks(); i++) {
        m_world->onTick();

        auto seconds = std::chrono::seconds(m_world->getSpentTime() + (std::time(NULL) - m_world->getLoadTime()));
        auto hours = std::chrono::duration_cast<std::chrono::hours>(seconds);
        seconds -= hours;
        auto minutes = duration_cast<std::chrono::minutes>(seconds);
        seconds -= minutes;

        Debug::get()->setString(DebugID::WORLD_TIME_SPENT, "Time spent in world: {} {} {}", hours, minutes, seconds);
    }

    m_player->update();

    if(m_online) {
        auto mp = Multiplayer::get();
        mp->update();

        if(mp->getState() == ERROR) {
            Game::get()->pushScene(std::make_shared<ErrorScene>(mp->getError()));
            mp->destroy();
            m_world->reset();
        }
    }

    if(IsKeyPressed(KEY_F3)) {
        auto dbg = Debug::get();
        dbg->setVisible(!dbg->isVisible());
    }

    if(IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    if(IsKeyPressed(KEY_F1)) {
        CloseWindow();
    }
}