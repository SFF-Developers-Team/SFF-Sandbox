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

MainMenuScene::MainMenuScene() : MenuBase() {
    auto sw = static_cast<float>(GetScreenWidth());
    auto game = Game::get();
    auto y = 300.f;
    auto const padding = 60.f;

    std::list<std::pair<std::string, MiniFunction<void()>>> const btns = {
        {"Singleplayer", [game]() { game->pushScene(std::make_shared<PlayScene>()); }},
        {"Multiplayer", [game]() { game->pushScene(std::make_shared<MultiplayerScene>()); }},
        {"Credits", [game]() { game->pushScene(std::make_shared<CreditsScene>()); }},
        {"Settings", [game]() { game->pushScene(std::make_shared<SettingsScene>()); }},
        {"Quit", [game]() { game->destroy(); }}
    };

    for(auto& [text, call] : btns) {
        auto btn = std::make_shared<Button>(text, call);
        btn->setPos({sw / 2, y});
        btn->setSize({300.f, 40.f});
        
        addChild(btn);
        
        y += padding;
    }
}