#include <world/gen/WorldGenNormal.hpp>
#include <RenderManager.hpp>
#include <entity/Player.hpp>
#include <ui/PlayScene.hpp>
#include <ui/ErrorScene.hpp>
#include <world/World.hpp>
#include <Multiplayer.hpp>
#include <StyleManager.hpp>
#include <Types.hpp>
#include <Debug.hpp>
#include <Timer.hpp>
#include <Game.hpp>
#include <ui/MainMenuScene.hpp>

#include <chrono>
#include <list>

PlayScene::PlayScene(bool isOnline) : Scene(), m_timer(std::make_shared<Timer>(60)), m_online(isOnline), m_paused(false) {
    auto game = Game::get();
    m_world = game->getWorld();
    m_player = game->getPlayer();
    m_color = COL_SKYBLUE;
    m_keyBack = false;

    if(!m_online) {
        if (!m_world->load()) {
            m_world->setGenerator(std::make_shared<WorldGenNormal>(m_world, 1));
            m_world->generate();
        }

        m_world->addPlayer(1, m_player);
    }

    HideCursor();

    auto pauseMenu = std::make_shared<Container>();
    pauseMenu->setFlag(FLAG_ALWAYS_CENTER, true);
    pauseMenu->setFlag(FLAG_GUI_SCALE, true);
    pauseMenu->setColor({0, 0, 0, 0});
    pauseMenu->setTag("pause-menu");
    pauseMenu->setBorderWidth(0.f);
    pauseMenu->setEnabled(false);
    pauseMenu->setVisible(false);
    addChild(pauseMenu);

    auto btnSize = StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE);
    auto border = StyleManager::get()->getValue<float>(DEFAULT_BORDER_WIDTH);

    std::list<std::pair<std::string, MiniFunction<void(Button*)>>> const btns = {
        {"Resume game", [this](auto) { resume(); }},
        {"Back to main menu", [game](auto) {
            game->pushScene(std::make_shared<MainMenuScene>());
            game->clearSceneHistory();
        }}
    };

    auto y = 0.f;
    for(auto& [text, call] : btns) {
        auto btn = std::make_shared<Button>(text, call);
        btn->setPos({0.f, y});
        btn->setAnchor({0.f, 0.f});
        pauseMenu->addChild(btn);
        
        y += btn->getHeight() + btn->getBorderWidth();
    }

    pauseMenu->hugContent(); 
}

PlayScene::~PlayScene() {
    if(!m_online) {
        m_world->save();
    }
}

void PlayScene::draw() {
    BeginMode2D(m_player->getCamera());
        RenderManager::renderWorld(m_world, m_player);
    EndMode2D();

    auto selectedBlock = m_player->getSelectedBlock();

    if (selectedBlock) {
        RenderManager::renderBlock({GetScreenWidth() - 42.f, 10.f, 32.f, 32.f}, selectedBlock);
    }

    Debug::get()->draw();

    auto mouse = GetMousePosition();
    RenderManager::drawTile("gui.png", 0, {mouse.x, mouse.y, 16.f, 16.f}, COL_WHITE, 0.f, {8.f, 8.f});

    if(m_paused) {
        RenderManager::drawRect({0.f, 0.f, getWidth(), getHeight()}, {0, 0, 0, 127});
    }

    Scene::draw();
}

void PlayScene::update() {
    m_timer->advanceTime();

    if(!m_paused) {
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
    }

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

    if(IsKeyPressed(KEY_ESCAPE)) {
        (m_paused ? resume() : pause());
    }

    Scene::update();
}

void PlayScene::pause() {
    auto pauseMenu = getChild<Container>("pause-menu");
    pauseMenu->setVisible(true);
    pauseMenu->setEnabled(true);
    m_paused = true;
    ShowCursor();
}

void PlayScene::resume() {
    auto pauseMenu = getChild<Container>("pause-menu");
    pauseMenu->setVisible(false);
    pauseMenu->setEnabled(false);
    m_paused = false;
    HideCursor();
}