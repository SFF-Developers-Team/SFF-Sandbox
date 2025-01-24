#include <ui/ErrorScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <Game.hpp>

ErrorScene::ErrorScene(std::string const& message) : m_message(message) {
    auto screenW = static_cast<float>(GetScreenWidth());
    auto screenH = static_cast<float>(GetScreenHeight());
    auto const buttonW = 200.f;

    auto error = std::make_shared<Text>("", 30.f);
    error->setPos({screenW / 2, screenH / 2});
    addChild(error);

    auto btn = std::make_shared<Button>("Main menu", [this]() {
        auto game = Game::get();
        game->clearSceneHistory();
        game->pushScene(std::make_shared<MainMenuScene>());
    });
    
    btn->setPos({screenW / 2, 460.f});
    btn->setSize({200.f, 40.f});
    addChild(btn);
}