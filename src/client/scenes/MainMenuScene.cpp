#include <scenes/MainMenuScene.hpp>
#include <scenes/PlayScene.hpp>
#include <scenes/MultiplayerScene.hpp>
#include <scenes/CreditsScene.hpp>
#include <scenes/SettingsScene.hpp>

#include <ui/basic/Button.hpp>
#include <ui/basic/ListContainer.hpp>

#include <managers/StyleManager.hpp>
#include <entity/LocalPlayer.hpp>
#include <Game.hpp>
#include <Platform.hpp>
#include <list>

MainMenuScene::MainMenuScene() : MenuBase() {
    auto game = Game::get();

    std::list<std::pair<std::string, MiniFunction<void(Button*)>>> const btns = {
        {"Singleplayer", [game](Button*) { 
            auto world = World::GenerateWorld(11111);
            auto player = std::make_shared<LocalPlayer>(*world);

            // TODO: World save

            world->addPlayer(player);
            world->loadPlayer();

            game->setWorld(std::move(world));
            game->setPlayer(player); 
            game->pushScene(std::make_shared<PlayScene>(game->getWorld(), false)); 
        }},
        {"Multiplayer", [game](Button*) { game->pushScene(std::make_shared<MultiplayerScene>()); }},
        {"Credits", [game](Button*) { game->pushScene(std::make_shared<CreditsScene>()); }},
        {"Settings", [game](Button*) { game->pushScene(std::make_shared<SettingsScene>()); }},
        {"Quit", [game](Button*) { game->close(); }}
    };

    auto container = std::make_shared<ListContainer>(false, true, true);
    container->setFlags(FLAG_ALWAYS_CENTER | FLAG_GUI_SCALE);
    container->setColor({0, 0, 0, 0});
    container->setBorderWidth(0.f);
    addChild(container);

    for(auto& [text, call] : btns) {
        container->addChild(std::make_shared<Button>(text, call));
    }
}