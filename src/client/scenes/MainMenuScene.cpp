#include <scenes/MainMenuScene.hpp>
#include <scenes/PlayScene.hpp>
#include <scenes/MultiplayerScene.hpp>
#include <scenes/CreditsScene.hpp>
#include <scenes/SettingsScene.hpp>

#include <ui/basic/Button.hpp>
#include <ui/basic/ListContainer.hpp>

#include <managers/StyleManager.hpp>
#include <world/ClientWorld.hpp>
#include <entity/LocalPlayer.hpp>
#include <world/gen/WorldGenNormal.hpp>
#include <Game.hpp>
#include <Platform.hpp>
#include <list>

MainMenuScene::MainMenuScene() : MenuBase() {
    auto game = Game::get();

    std::list<std::pair<std::string, MiniFunction<void(Button*)>>> const btns = {
        {"Singleplayer", [game](Button*) { 
            auto world = std::make_shared<ClientWorld>(Platform::getHomeDir() / "worlds" / "world1");

            srand(time(0));

            // if (!world->load()) {
                world->setGenerator(std::make_shared<WorldGenNormal>(rand()));
                world->generate();
            // }

            auto player = std::make_shared<LocalPlayer>(world);

            game->setWorld(world);
            game->setPlayer(player);
            world->addPlayer(player);
            world->loadPlayer();

            // I HATE THIS SHIT
            // WHEN I CLEAR SCENE HISTORY, PLAYSCENE CAUSES CRASH
            game->pushScene(std::make_shared<PlayScene>()); 
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