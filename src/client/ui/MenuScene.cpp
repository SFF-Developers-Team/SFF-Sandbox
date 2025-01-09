#include <ui/MenuScene.hpp>
#include <ui/PlayScene.hpp>
#include <ui/MultiplayerScene.hpp>
#include <ui/CreditsScene.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <TileMap.hpp>
#include <raygui.h>
#include <functional>
#include <utility>
#include <Game.hpp>
#include <array>

auto const bgwidth = 11;
auto const bgheight = 6;

MenuScene::MenuScene() : m_bgRender(LoadRenderTexture(bgwidth * 16, bgheight * 16)) {
    m_bgColor = COL_SKYBLUE;

    auto game = Game::get();
    // TODO: Make tilemap manager
    // TODO: Merge all managers into AssetsManager
    auto blocksMap = game->getBlocksTileMap();

    // 0 - AIR; 1 - GRASS; 2 - DIRT; 3 - STONE
    std::array<uint8_t, bgwidth * bgheight> blocks = {
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0,
        2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0,
        3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1
    };

    BeginTextureMode(m_bgRender);
        ClearBackground({0, 0, 0, 0});

        for(int i = 0; i < blocks.size(); i++) {
            if(blocks[i] != 0) {
                auto x = static_cast<float>(i % bgwidth);
                auto y = static_cast<float>(i / bgwidth);

                blocksMap->drawTilePro(blocks[i] - 1, {x * 16, y * 16, 16, 16}, WHITE);
            }
        }

        auto player = TextureManager::get()->getTexture("player.png");

        RenderManager::get()->renderPlayerTexture({70, 14}, "player.png", {16.f, 22.f}, 15, RIGHT);
    EndTextureMode();
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

    DrawTexturePro(
        m_bgRender.texture, 
        {0, 0, (float)m_bgRender.texture.width, (float)-m_bgRender.texture.height},
        // what??? constant values without calculation???
        {-47, 235, 885, 485}, {0, 0}, 0, WHITE
    );

    drawMenuLogo();

    drawButtonsV({sw / 2.f - btnSize.x / 2.f, 300.f}, btnSize, 60.f, {
        {"Play", [&](){ game->pushScene(std::make_shared<PlayScene>(world, player)); }},
        {"Multiplayer", [&](){ game->pushScene(std::make_shared<MultiplayerScene>()); }},
        {"Credits", [&](){ game->pushScene(std::make_shared<CreditsScene>()); }},
        {"Settings", [&](){ /* game->pushScene(std::make_shared<SettingsScene>()); */ }}
    });
}