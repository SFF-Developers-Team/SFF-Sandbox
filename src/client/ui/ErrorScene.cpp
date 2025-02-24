#include <ui/ErrorScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <Game.hpp>

ErrorScene::ErrorScene(std::string const& message) : m_message(message) {
    auto const buttonW = 200.f;

    auto error = std::make_shared<Text>("font", "", 30.f);
    error->setFlags(FLAG_GUI_SCALE | FLAG_ALWAYS_CENTER);
    error->setPos({getWidth() / 2, getHeight() / 2});
    addChild(error);

    auto btn = std::make_shared<Button>("Main menu", [this](Button*) {
        auto game = Game::get();
        game->clearSceneHistory();
        game->pushScene(std::make_shared<MainMenuScene>());
    });
    
    btn->setPos({getWidth() / 2, error->getY() + error->getScaledHeight()});
    btn->setFlags(FLAG_GUI_SCALE);
    addChild(btn);
}
