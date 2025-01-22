#include <ui/ErrorScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <Game.hpp>

ErrorScene::ErrorScene(std::string const& message) : m_message(message) {}

void ErrorScene::draw() {
    MenuBase::draw();

    auto screenW = static_cast<float>(GetScreenWidth());
    auto screenH = static_cast<float>(GetScreenHeight());
    auto const buttonW = 200.f;

    // drawText(m_message, {screenW / 2.f, screenH / 2.f}, 28.f, true);

    // drawButton("Main menu", {screenW / 2.f - buttonW / 2.f, 460.f, 200.f, 40.f}, []() -> void {
    //     auto game = Game::get();
    //     game->clearSceneHistory();
    //     game->pushScene(std::make_shared<MainMenuScene>());
    // });
}