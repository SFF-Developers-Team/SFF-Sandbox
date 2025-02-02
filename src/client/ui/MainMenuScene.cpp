#include <ui/MainMenuScene.hpp>
#include <ui/PlayScene.hpp>
#include <ui/MultiplayerScene.hpp>
#include <ui/CreditsScene.hpp>
#include <ui/SettingsScene.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <Game.hpp>
#include <ui/nodes/Button.hpp>
#include <list>
#include <ui/nodes/List.hpp>

MainMenuScene::MainMenuScene() : MenuBase() {
    auto game = Game::get();
    auto y = 300.f;
    auto const padding = 60.f;

    std::list<std::pair<std::string, MiniFunction<void(Button*)>>> const btns = {
        {"Singleplayer", [game](Button*) { game->pushScene(std::make_shared<PlayScene>()); }},
        {"Multiplayer", [game](Button*) { game->pushScene(std::make_shared<MultiplayerScene>()); }},
        {"Credits", [game](Button*) { game->pushScene(std::make_shared<CreditsScene>()); }},
        {"Settings", [game](Button*) { game->pushScene(std::make_shared<SettingsScene>()); }},
        {"Quit", [game](Button*) { game->destroy(); }}
    };

    for(auto& [text, call] : btns) {
        auto btn = std::make_shared<Button>(text, call);
        btn->setPos({getWidth() / 2, y});
        btn->setSize({300.f, 40.f});
        
        addChild(btn);
        
        y += padding;
    }
}