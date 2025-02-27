#include <ui/ErrorScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <Game.hpp>

ErrorScene::ErrorScene(std::string const& message) : MenuBase() {
    auto error = std::make_shared<Text>("font", message);
    error->setFlags(FLAG_GUI_SCALE | FLAG_ALWAYS_CENTER);
    error->setPos({getWidth() / 2, getHeight() / 2});
    addChild(error);

    auto btn = std::make_shared<Button>("Main menu", [this](Button*) { destroy(); });
    btn->setPos({getWidth() / 2, error->getBottomY() + 40.f});
    btn->setFlags(FLAG_GUI_SCALE);
    btn->setAnchorY(0.f);
    addChild(btn);
}

void ErrorScene::onPush() {
    MenuBase::onPush();
    Game::get()->clearSceneHistory();
}