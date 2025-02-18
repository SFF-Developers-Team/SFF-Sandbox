#include <ui/MainMenuScene.hpp>
#include <ui/PlayScene.hpp>
#include <ui/MultiplayerScene.hpp>
#include <ui/CreditsScene.hpp>
#include <ui/SettingsScene.hpp>
#include <ui/nodes/Button.hpp>
#include <StyleManager.hpp>
#include <Game.hpp>
#include <list>

MainMenuScene::MainMenuScene() : MenuBase() {
    auto game = Game::get();

    std::list<std::pair<std::string, MiniFunction<void(Button*)>>> const btns = {
        {"Singleplayer", [game](Button*) { game->pushScene(std::make_shared<PlayScene>()); }},
        {"Multiplayer", [game](Button*) { game->pushScene(std::make_shared<MultiplayerScene>()); }},
        {"Credits", [game](Button*) { game->pushScene(std::make_shared<CreditsScene>()); }},
        {"Settings", [game](Button*) { game->pushScene(std::make_shared<SettingsScene>()); }},
        {"Quit", [game](Button*) { game->destroy(); }}
    };

    auto container = std::make_shared<Container>();
    container->setFlag(FLAG_ALWAYS_CENTER, true);
    container->setFlag(FLAG_GUI_SCALE, true);
    container->setColor({0, 0, 0, 0});
    container->setBorderWidth(0.f);
    addChild(container);

    auto y = 0.f;
    for(auto& [text, call] : btns) {
        auto btn = std::make_shared<Button>(text, call);
        btn->setAnchor({0.f, 0.f});
        btn->setPos({0.f, y});
        container->addChild(btn);
        
        y += btn->getBorderWidth() + btn->getHeight();
    }
    
    container->hugContent();
}