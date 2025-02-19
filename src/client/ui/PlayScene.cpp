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
#include <ui/nodes/Hotbar.hpp>
#include <ui/nodes/Inventory.hpp>
#include <chrono>
#include <list>

PlayScene::PlayScene(bool isOnline) : Scene(), m_timer(std::make_shared<Timer>(60)), m_online(isOnline), m_paused(false), m_inventoryEnabled(false) {
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
        {"Resume game", [this](auto) { setPaused(false); }},
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

    auto hotbar = std::make_shared<Hotbar>(m_player);
    hotbar->setAnchorY(0.f);
    hotbar->setPos({getWidth() / 2, 0.f});
    hotbar->setFlag(FLAG_GUI_SCALE, true);
    // hotbar->setTag("hotbar");
    addChild(hotbar);

    auto inventory = std::make_shared<Inventory>(m_player->getInventory());
    inventory->setFlag(FLAG_GUI_SCALE, true);
    inventory->setFlag(FLAG_ALWAYS_CENTER, true);
    inventory->setVisible(false);
    inventory->setEnabled(false);
    inventory->setTag("inventory");
    addChild(inventory);
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

    if(m_paused) {
        RenderManager::drawRect({0.f, 0.f, getWidth(), getHeight()}, {0, 0, 0, 127});
    }

    Scene::draw();

    auto mouse = GetMousePosition();
    RenderManager::drawTile("gui.png", 0, {mouse.x, mouse.y, 16.f, 16.f}, COL_WHITE, 0.f, {8.f, 8.f});

    Debug::get()->draw();
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

        if(!m_inventoryEnabled) {
            m_player->updateControls();
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

    if(IsKeyPressed(KEY_ESCAPE) && !m_inventoryEnabled) {
        setPaused(!m_paused);
    }

    if(IsKeyPressed(KEY_ESCAPE) && m_inventoryEnabled) {
        setEnabledInventory(false);
    }

    if(IsKeyPressed(KEY_E)) {
        setEnabledInventory(!m_inventoryEnabled);
    }

    Scene::update();
}

void PlayScene::setPaused(bool paused) {
    auto pauseMenu = getChild<Container>("pause-menu");
    pauseMenu->setVisible(paused);
    pauseMenu->setEnabled(paused);
    m_paused = paused;
    (paused ? ShowCursor() : HideCursor());
}

void PlayScene::setEnabledInventory(bool isOpen) {
    auto inventory = getChild<Inventory>("inventory");
    inventory->setVisible(isOpen);
    inventory->setEnabled(isOpen);
    m_inventoryEnabled = isOpen;
}