#include <ui/MenuScene.hpp>
#include <ui/PlayScene.hpp>
#include <ui/MultiplayerScene.hpp>
#include <ui/CreditsScene.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <raygui.h>
#include <functional>
#include <utility>
#include <Game.hpp>

MenuScene::MenuScene() {
    m_bgColor = COL_SKYBLUE;
}

void MenuScene::draw() {
    auto sw = static_cast<float>(GetScreenWidth());
    auto sh = static_cast<float>(GetScreenHeight());
    auto rm = RenderManager::get();
    auto tm = TextureManager::get();

    auto game = Game::get();
    auto world = game->getWorld();
    auto player = game->getPlayer();

    Vec2f const btnSize = {300.f, 40.f};

    drawMenuLogo();

    drawButtonsV({sw / 2.f - btnSize.x / 2.f, 300.f}, btnSize, 60.f, {
        {"Play", [&](){ game->pushScene(std::make_shared<PlayScene>(world, player)); }},
        {"Multiplayer", [&](){ game->pushScene(std::make_shared<MultiplayerScene>()); }},
        {"Credits", [&](){ game->pushScene(std::make_shared<CreditsScene>()); }},
        {"Settings", [&](){ /* game->pushScene(std::make_shared<SettingsScene>()); */ }}
    });
}