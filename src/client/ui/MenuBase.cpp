#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <ui/MenuBase.hpp>
#include <TileMap.hpp>
#include <Types.hpp>
#include <Game.hpp>

#include <array>

auto const bgwidth = 11;
auto const bgheight = 6;

MenuBase::MenuBase() : m_bgRender(LoadRenderTexture(bgwidth * 16, bgheight * 16)) {
    m_bgColor = COL_SKYBLUE;

    auto game = Game::get();
    auto rm = RenderManager::get();

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

                // rm->drawTile("blocks.png", blocks[i] - 1, {x * 16, y * 16, 16, 16}, COL_WHITE);
            }
        }

        auto player = TextureManager::get()->getTexture("player.png");

        RenderManager::get()->renderPlayerTexture({70, 14}, "player.png", {16.f, 22.f}, 15, RIGHT);
    EndTextureMode();
}

MenuBase::~MenuBase() {
    UnloadRenderTexture(m_bgRender);
}

void MenuBase::draw() {
    Scene::draw();

    auto w = static_cast<float>(m_bgRender.texture.width) * 5.05f;
    auto h = static_cast<float>(m_bgRender.texture.height) * 5.05f;

    DrawTexturePro(
        m_bgRender.texture, 
        {0, 0, (float)m_bgRender.texture.width, (float)-m_bgRender.texture.height},
        // what??? constant values without calculation???
        {-47.f, static_cast<float>(GetScreenHeight()) - h, w, h}, {0, 0}, 0, WHITE
    );

    drawMenuLogo();
}