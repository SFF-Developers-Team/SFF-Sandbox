#include <ui/MainMenuScene.hpp>
#include <ui/PlayScene.hpp>
#include <ui/MultiplayerScene.hpp>
#include <ui/CreditsScene.hpp>
#include <ui/SettingsScene.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <Game.hpp>

MainMenuScene::MainMenuScene() : MenuBase() {}

void MainMenuScene::draw() {
    MenuBase::draw();

    auto sw = static_cast<float>(GetScreenWidth());
    auto sh = static_cast<float>(GetScreenHeight());
    auto rm = RenderManager::get();
    auto tm = TextureManager::get();

    auto game = Game::get();

    Vec2f const btnSize = {300.f, 40.f};

    drawButtonsV({sw / 2.f - btnSize.x / 2.f, 300.f}, btnSize, 60.f, {
        {"Play", [&](){ game->pushScene(std::make_shared<PlayScene>()); }},
        {"Multiplayer", [&](){ game->pushScene(std::make_shared<MultiplayerScene>()); }},
        {"Credits", [&](){ game->pushScene(std::make_shared<CreditsScene>()); }},
        {"Settings", [&](){ game->pushScene(std::make_shared<SettingsScene>()); }},
        {"Exit", [&](){ CloseWindow(); }}
    });
}