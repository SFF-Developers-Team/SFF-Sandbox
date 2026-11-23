#include <raylib-cpp.hpp>
#include <scenes/PlayScene.hpp>
#include <scenes/ErrorScene.hpp>
#include <scenes/MainMenuScene.hpp>

#include <ui/basic/List.hpp>
#include <ui/basic/Text.hpp>
#include <ui/basic/Layer.hpp>
#include <ui/basic/Button.hpp>
#include <ui/basic/TextInput.hpp>
#include <ui/basic/ListContainer.hpp>
#include <ui/basic/TouchControlButton.hpp>

#include <ui/gameplay/Hotbar.hpp>
#include <ui/gameplay/TileInfo.hpp>
#include <ui/gameplay/InventoryNode.hpp>
#include <ui/gameplay/HeartsIndicator.hpp>
#include <ui/gameplay/IngredientsList.hpp>

#include <entity/LocalPlayer.hpp>
#include <world/ClientWorld.hpp>

#include <managers/SettingsManager.hpp>
#include <managers/RecipesManager.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/StyleManager.hpp>
#include <net/Multiplayer.hpp>
#include <Types.hpp>
#include <managers/Debug.hpp>
#include <Timer.hpp>
#include <Game.hpp>
#include <Item.hpp>

PlayScene::PlayScene(bool isOnline) : Scene(), m_timer(std::make_shared<Timer>(60)), m_online(isOnline), m_paused(false), m_inventoryEnabled(false) {
    auto game = Game::get();
    m_world = game->getWorld();
    m_player = game->getPlayer();
    m_color = raylib::Color::SkyBlue();
    m_keyBack = false;

    game->clearSceneHistory();

    raylib::Window::HideCursor();
    
    auto hotbar = std::make_shared<Hotbar>(m_player, [this]() { setInventoryOpened(!m_inventoryEnabled); });
    hotbar->setAnchorY(0.f);
    hotbar->setPos({getWidth() / 2, 0.f});
    hotbar->setFlags(FLAG_GUI_SCALE);
    hotbar->setTag("hotbar");
    addChild(hotbar);

    auto tileInfo = std::make_shared<TileInfo>();
    tileInfo->setFlags(FLAG_GUI_SCALE);
    tileInfo->setAnchorY(0.f);
    tileInfo->setVisible(false);
    tileInfo->setEnabled(false);
    tileInfo->setPos({getWidth() / 2, hotbar->getHeight() * getGlobalScaleY() + 10.f});
    tileInfo->setTag("tileinfo");
    addChild(tileInfo);

    auto hp = std::make_shared<HeartsIndicator>(m_player);
    hp->setAnchor({1.f, 0.f});
    hp->setPos({getWidth() - 5.f, 5.f});
    hp->setFlags(FLAG_GUI_SCALE);
    addChild(hp);

    // oh fuck we should create InventoryContainer class and move this shit to it
    auto inventoryContainer = std::make_shared<Container>();
    inventoryContainer->setFlags(FLAG_GUI_SCALE | FLAG_ALWAYS_CENTER);
    inventoryContainer->setVisible(false);
    inventoryContainer->setEnabled(false);
    inventoryContainer->setTag("inventory");
    addChild(inventoryContainer);

    auto border = inventoryContainer->getBorderWidth();
    auto inventory = std::make_shared<InventoryNode>(m_player);
    inventory->setPos({border * 2, border * 2});
    inventory->setAnchor({0.f, 0.f});
    inventoryContainer->addChild(inventory);

    auto invLabel = std::make_shared<Text>("boldfont", "Inventory");
    invLabel->setPos({border, border});
    invLabel->setAnchor({0.f, 0.f});
    invLabel->setWidth(inventory->getWidth());
    inventory->setY(invLabel->getBottomY());
    inventoryContainer->addChild(invLabel);


    std::vector<std::string> recipesList;

    // for (auto& recipe : RecipesManager::get()->getRecipes()) {
    //     recipesList.push_back(recipe.result->getName());
    // }

    auto craftList = std::make_shared<List>(recipesList, [inventoryContainer](auto, int id) {
        auto ingredients = inventoryContainer->getChild<IngredientsList>("ingredients");
        auto recipes = RecipesManager::get()->getRecipes();

        if (ingredients != nullptr) {
            ingredients->setRecipe(recipes[id]);
        }
    });
    craftList->setPos({inventory->getRightX() + border, border * 2});
    craftList->setAnchor({0.f, 0.f});
    craftList->setSize({craftList->getWidth() / 2, inventory->getHeight()});
    inventoryContainer->addChild(craftList);

    auto craftLabel = std::make_shared<Text>("boldfont", "Craft");
    craftLabel->setPos({craftList->getX(), border});
    craftLabel->setAnchor({0.f, 0.f});
    craftLabel->setWidth(craftList->getWidth());
    craftList->setY(craftLabel->getBottomY());
    inventoryContainer->addChild(craftLabel);

    auto ingredients = std::make_shared<IngredientsList>();
    ingredients->setPos({inventory->getX(), inventory->getBottomY() + border});
    ingredients->setAnchor({0.f, 0.f});
    ingredients->setWidth(inventory->getWidth());
    ingredients->setTag("ingredients");
    inventoryContainer->addChild(ingredients);

    auto craftBtn = std::make_shared<Button>("Craft!", [this, craftList](auto) {
        auto selected = craftList->getSelected();
        auto rm = RecipesManager::get();
        auto recipes = rm->getRecipes();

        rm->craftItem(*m_player, recipes[selected]);
    });
    craftBtn->setPos({craftList->getX(), craftList->getBottomY() + border});
    craftBtn->setAnchor({0.f, 0.f});
    craftBtn->setSize({craftList->getWidth(), ingredients->getHeight()});
    inventoryContainer->addChild(craftBtn);
    inventoryContainer->setSize({craftList->getRightX() + border * 2, ingredients->getBottomY() + border * 2});

    auto pauseLayer = std::make_shared<Layer>();
    pauseLayer->setColor({0, 0, 0, 64});
    pauseLayer->setEnabled(false);
    pauseLayer->setVisible(false);
    pauseLayer->setTag("pause-menu");
    addChild(pauseLayer);

    auto pauseMenu = std::make_shared<ListContainer>(false, false, true);
    pauseMenu->setFlags(FLAG_ALWAYS_CENTER | FLAG_GUI_SCALE);
    pauseMenu->setColor({0, 0, 0, 0});
    pauseMenu->setBorderWidth(0.f);
    pauseMenu->addChild(std::make_shared<Button>("Resume game", [this](auto) { setPaused(false); }));
    pauseMenu->addChild(std::make_shared<Button>("Back to main menu", [this](auto) { destroy(); }));
    pauseLayer->addChild(pauseMenu);

    m_pauseNodes = {hotbar, tileInfo};

    if(m_online) {
        auto playerList = std::make_shared<List>(nickList, [](auto, auto) {});
        playerList->setFlags(FLAG_GUI_SCALE);
        playerList->setPos({static_cast<float>(raylib::Window::GetWidth() / 2.f + 735), 0});
        playerList->setVisible(true);
        playerList->setEnabled(true);
        playerList->setAnchorY(0.f);
        playerList->setSize({150, static_cast<float>(raylib::Window::GetHeight() / Game::get()->getGuiScale())});
        playerList->setTag("playerlist");
        m_playersList = playerList;
        pauseLayer->addChild(m_playersList);

        auto msgInput = std::make_shared<TextInput>("font", "");
        msgInput->setFlags(FLAG_GUI_SCALE);
        msgInput->setPos(raylib::Window::GetSize() / 2);
        msgInput->setTag("inputmsg");
        addChild(msgInput);
    }
}

PlayScene::~PlayScene() {
    if(!m_online) {
        m_world->save();
    }
}

void PlayScene::draw() {
    raylib::Vector2 static mouse = {0};
    TilePosition static target = {0};
    auto& rm = ResourceManager::Get();

    if (!m_paused) {
        mouse = raylib::Mouse::GetPosition();
        target = m_player->getTargetTile();
    }

    m_player->getCamera().BeginMode();
        m_world->draw(m_player);

        // if (m_player->isBreakingTile()) {
        //     auto breakingTile = m_player->getBreakingTileInfo(m_world.get());
    
        //     rm.GetTilemap("gui.png").DrawTile(8 + (5.f - (breakingTile.currentDurability / breakingTile.totalDurability) * 5.f), TILE_RECT(target.x, target.y));
        // }

        if (m_player->canAccessTile(target)) {
            if(m_player->canPlaceTile(*m_world, target)) {
                rm.GetTilemap("gui.png").DrawTile(1, TILE_RECT(target.x, target.y));
            }

            if(m_player->canDestroyTile(*m_world, target)) {
                rm.GetTilemap("gui.png").DrawTile(2 + raylib::Keyboard::IsKeyDown(KEY_LEFT_CONTROL), TILE_RECT(target.x, target.y));
            }
        }
    m_player->getCamera().EndMode();

    if(m_online) {
        auto mp = Multiplayer::get();
        auto messages = mp->getMessages();

        for(int i = 0; i < messages.size(); i++) {
            rm.GetFont("font").DrawText(messages[i], {0, static_cast<float>(10 * i)}, 10.f, 1.f, raylib::Color::White());
        }
    }

    rm.GetTilemap("gui.png").DrawTile(0, raylib::Rectangle{mouse.x, mouse.y, 16.f, 16.f}, raylib::Color::White(), 0.f, {8.f, 8.f});

    if(m_paused) {
        RRectangle(getSize()).Draw(raylib::Color{0, 0, 0, 127});
    }

    Scene::draw();
    Debug::get()->draw();
}

void PlayScene::update() {
    m_timer->advanceTime();

    if(!m_paused) {
        for (uint32_t i = 0; i < m_timer->getTicks(); i++) {
            m_world->onTick();

            auto time = m_world->getTime();
            auto seconds = (time / 60) % 60;
            auto minutes = (time / 3600) % 60;
            auto hours = time / 216000;

            Debug::get()->setString(DebugID::WORLD_TIME_SPENT, "Time spent in world: {}h {}m {}s", hours, minutes, seconds);
        }

        if(!m_inventoryEnabled) {
            m_player->updateControls();
        }

        m_player->update();
    }

    
    auto stm = SettingsManager::get();
    auto target = m_player->getTargetTile();
    auto tile = m_world->getTile(target);
    auto tileInfo = getChild<TileInfo>("tileinfo");
    auto playerList = getChild<List>("playerlist"); 
    auto inputMsg = getChild<TextInput>("inputmsg");

    // auto enabledTileInfo = stm->getValue<bool>("video.tileinfo", true) && tile != nullptr && !m_inventoryEnabled && !m_paused;

    // tileInfo->setVisible(enabledTileInfo);
    // tileInfo->setEnabled(enabledTileInfo);

    if (playerList != nullptr) {
        playerList->setVisible(m_paused);
        playerList->setEnabled(m_paused);
    }

    if(m_online) {
        auto mp = Multiplayer::get();
        mp->update();

        if(mp->getState() == MultiplayerState::ERROR) {
            Game::get()->pushScene(std::make_shared<ErrorScene>(mp->getError()));
            mp->destroy();
        }

        if(raylib::Keyboard::IsKeyPressed(KEY_ENTER)) {
            if(!inputMsg->getText().empty()) {
                auto msg = inputMsg->getText();
                auto pak = Packet(ObjectHeader::MESSAGE, msg);
                mp->sendPacket(pak);
                m_world = nullptr;
            }
        }
    }

    if(raylib::Keyboard::IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    if(raylib::Keyboard::IsKeyPressed(KEY_F1)) {
        Game::get()->close();
    }

    if(raylib::Keyboard::IsKeyPressed(KEY_ESCAPE) && !m_inventoryEnabled) {
        setPaused(!m_paused);
    }

    if(raylib::Keyboard::IsKeyPressed(KEY_ESCAPE) && m_inventoryEnabled) {
        setInventoryOpened(false);
    }

    if(raylib::Keyboard::IsKeyPressed(KEY_E) && !m_paused) {
        setInventoryOpened(!m_inventoryEnabled);
    }

    Scene::update();
}

void PlayScene::setPaused(bool paused) {
    auto pauseMenu = getChild<Container>("pause-menu");

    pauseMenu->setVisible(paused);  
    pauseMenu->setEnabled(paused);
    
    if(m_online) {
        auto players = m_world->getPlayers();
        if(nickList.size() != players.size() && m_playersList != nullptr) {
            nickList.clear();
            nickList.push_back(Game::get()->getUsername());
            
            for(auto& [id, player] : players) {
                if(player->getUsername().empty()) {
                    continue;
                }
                nickList.push_back(player->getUsername());
            }
            
            m_playersList->setList(nickList);  
        }
    }

    m_paused = paused;
    (paused ? raylib::Window::ShowCursor() : raylib::Window::HideCursor());

    for (auto& node : m_pauseNodes) {
        node->setEnabled(!paused);
    }
}

void PlayScene::setInventoryOpened(bool isOpen) {
    auto inventory = getChild<Container>("inventory");
    inventory->setVisible(isOpen);
    inventory->setEnabled(isOpen);
    m_inventoryEnabled = isOpen;

    (isOpen ? raylib::Window::ShowCursor() : raylib::Window::HideCursor());
}

void PlayScene::keyBackClicked() {
    (!m_inventoryEnabled) ? setPaused(!m_paused) : setInventoryOpened(false);
}

void PlayScene::onPop() {
    raylib::Window::ShowCursor();
}