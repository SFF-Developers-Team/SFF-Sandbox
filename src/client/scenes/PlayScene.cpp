#include <world/gen/WorldGenNormal.hpp>
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
#include <ui/gameplay/BlockInfo.hpp>
#include <ui/gameplay/InventoryNode.hpp>
#include <ui/gameplay/HeartsIndicator.hpp>
#include <ui/gameplay/IngredientsList.hpp>

#include <entity/LocalPlayer.hpp>
#include <world/ClientWorld.hpp>

#include <managers/SettingsManager.hpp>
#include <managers/RecipesManager.hpp>
#include <managers/RenderManager.hpp>
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
    m_color = COL_SKYBLUE;
    m_keyBack = false;

    game->clearSceneHistory();

    HideCursor();
    
    auto hotbar = std::make_shared<Hotbar>(m_player, [this]() { setInventoryOpened(!m_inventoryEnabled); });
    hotbar->setAnchorY(0.f);
    hotbar->setPos({getWidth() / 2, 0.f});
    hotbar->setFlags(FLAG_GUI_SCALE);
    hotbar->setTag("hotbar");
    addChild(hotbar);

    auto blockInfo = std::make_shared<BlockInfo>();
    blockInfo->setFlags(FLAG_GUI_SCALE);
    blockInfo->setAnchorY(0.f);
    blockInfo->setVisible(false);
    blockInfo->setEnabled(false);
    blockInfo->setPos({getWidth() / 2, hotbar->getHeight() * getGlobalScaleY() + 10.f});
    blockInfo->setTag("blockinfo");
    addChild(blockInfo);

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

    for (auto& recipe : RecipesManager::get()->getRecipes()) {
        recipesList.push_back(recipe.result->getName());
    }

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

        rm->craftItem(m_player, recipes[selected]);
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

    m_pauseNodes = {hotbar, blockInfo};

    if(m_online) {
        auto playerList = std::make_shared<List>(nickList, [](auto, auto) {});
        playerList->setFlags(FLAG_GUI_SCALE);
        playerList->setPos({static_cast<float>(GetScreenWidth() / 2.f + 735), 0});
        playerList->setVisible(true);
        playerList->setEnabled(true);
        playerList->setAnchorY(0.f);
        playerList->setSize({150, static_cast<float>(GetScreenHeight() / Game::get()->getGuiScale())});
        playerList->setTag("playerlist");
        m_playersList = playerList;
        pauseLayer->addChild(m_playersList);

        auto msgInput = std::make_shared<TextInput>("font", "");
        msgInput->setFlags(FLAG_GUI_SCALE);
        msgInput->setPos({static_cast<float>(GetScreenWidth() / 2.f), static_cast<float>(GetScreenHeight() / 2.f)});
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
    Vector2 static mouse = {0};
    BlockPosition static target = {0};

    if (!m_paused) {
        mouse = GetMousePosition();
        target = m_player->getTargetBlock();
    }

    BeginMode2D(m_player->getCamera());
        m_world->draw(m_player);

        // if (m_player->isBreakingBlock()) {
        //     auto breakingBlock = m_player->getBreakingBlockInfo(m_world.get());
    
        //     RenderManager::drawTile("gui.png", 8 + (5.f - (breakingBlock.currentDurability / breakingBlock.totalDurability) * 5.f), BLOCK_RECT(target.x, target.y));
        // }

        if (m_player->canAccessBlock(target)) {
            if(m_player->canPlaceBlock(m_world.get(), target)) {
                RenderManager::drawTile("gui.png", 1, BLOCK_RECT(target.x, target.y));
            }

            if(m_player->canDestroyBlock(m_world.get(), target)) {
                RenderManager::drawTile("gui.png", 2 + IsKeyDown(KEY_LEFT_CONTROL), BLOCK_RECT(target.x, target.y));
            }
        }
    EndMode2D();

    if(m_online) {
        auto mp = Multiplayer::get();
        auto messages = mp->getMessages();

        for(int i = 0; i < messages.size(); i++) {
            RenderManager::drawText("font", messages[i], {0, static_cast<float>(10 * i)});
        }
    }

    RenderManager::drawTile("gui.png", 0, {mouse.x, mouse.y, 16.f, 16.f}, COL_WHITE, 0.f, {0.5f, 0.5f});

    if(m_paused) {
        RenderManager::drawRect({0.f, 0.f, getWidth(), getHeight()}, {0, 0, 0, 127});
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
    auto target = m_player->getTargetBlock();
    auto block = m_world->getBlock(target);
    auto blockInfo = getChild<BlockInfo>("blockinfo");
    auto playerList = getChild<List>("playerlist"); 
    auto inputMsg = getChild<TextInput>("inputmsg");

    auto enabledBlockInfo = stm->getValue<bool>("video.blockinfo", true) && block != nullptr && !m_inventoryEnabled && !m_paused;

    blockInfo->setVisible(enabledBlockInfo);
    blockInfo->setEnabled(enabledBlockInfo);

    if (playerList != nullptr) {
        playerList->setVisible(m_paused);
        playerList->setEnabled(m_paused);
    }

    if(m_online) {
        auto mp = Multiplayer::get();
        mp->update();

        if(mp->getState() == ERROR) {
            Game::get()->pushScene(std::make_shared<ErrorScene>(mp->getError()));
            mp->destroy();
        }

        if(IsKeyPressed(KEY_ENTER)) {
            if(!inputMsg->getText().empty()) {
                auto msg = inputMsg->getText();
                auto pak = Packet(ObjectHeader::MESSAGE, msg);
                mp->sendPacket(pak);
                m_world = nullptr;
            }
        }
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
        setInventoryOpened(false);
    }

    if(IsKeyPressed(KEY_E) && !m_paused) {
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
    (paused ? ShowCursor() : HideCursor());

    for (auto& node : m_pauseNodes) {
        node->setEnabled(!paused);
    }
}

void PlayScene::setInventoryOpened(bool isOpen) {
    auto inventory = getChild<Container>("inventory");
    inventory->setVisible(isOpen);
    inventory->setEnabled(isOpen);
    m_inventoryEnabled = isOpen;

    (isOpen ? ShowCursor() : HideCursor());
}

void PlayScene::keyBackClicked() {
    (!m_inventoryEnabled) ? setPaused(!m_paused) : setInventoryOpened(false);
}

void PlayScene::onPop() {
    ShowCursor();
}