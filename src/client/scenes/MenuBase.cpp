#include <scenes/MenuBase.hpp>
#include <managers/TextureManager.hpp>
#include <managers/RenderManager.hpp>
#include <TileMap.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <array>

auto const bgwidth = 11;
auto const bgheight = 10;
RenderTexture2D m_bgRender;

MenuBase::MenuBase() : Scene() {
    m_color = COL_SKYBLUE;
    
    if(m_bgRender.id == 0) {
        auto game = Game::get();

        m_bgRender = LoadRenderTexture(bgwidth * 16, bgheight * 16);
    
        // 0 - AIR; 1 - GRASS; 2 - DIRT; 3 - STONE
        std::array<uint8_t, bgwidth * bgheight> blocks = {
            0,  0, 11, 11, 11,  0, 0, 0, 0, 0, 0,
            0, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0,
            0, 11, 11,  9, 11, 11, 0, 0, 0, 0, 0,
            0,  0, 11,  9, 11,  0, 0, 0, 0, 0, 0,
            1,  1,  0,  9,  0,  0, 0, 0, 0, 0, 0,
            2,  2,  1,  9,  0,  0, 0, 0, 0, 0, 0,
            2,  2,  2,  1,  1,  0, 0, 0, 0, 0, 0,
            2,  2,  2,  2,  2,  1, 1, 0, 0, 0, 0,
            2,  2,  2,  2,  2,  2, 2, 1, 1, 0, 0,
            3,  3,  3,  2,  2,  2, 2, 2, 2, 1, 1
        };

        BeginTextureMode(m_bgRender);
            ClearBackground({0, 0, 0, 0});

            for(int i = 0; i < blocks.size(); i++) {
                if(blocks[i] != 0) {
                    auto color = (blocks[i] == 11) ? COL_GREEN : COL_WHITE;
                    auto x = static_cast<float>(i % bgwidth);
                    auto y = static_cast<float>(i / bgwidth);

                    RenderManager::drawTile("blocks.png", blocks[i] - 1, {x * 16, y * 16, 16, 16}, color);
                }
            }

            auto player = TextureManager::get()->getTexture("player.png");

            RenderManager::renderPlayerTexture({70, 78}, "player.png", {16.f, 22.f}, 15, RIGHT);
        EndTextureMode();
    }
}

void MenuBase::draw() {
    auto tm = TextureManager::get();
    auto scale = Game::get()->getGuiScale();
    auto w = static_cast<float>(m_bgRender.texture.width) * 2.5f * scale;
    auto h = static_cast<float>(m_bgRender.texture.height) * 2.5f * scale;

    DrawTexturePro(
        m_bgRender.texture, 
        {0, 0, (float)m_bgRender.texture.width, (float)-m_bgRender.texture.height},
        {-32.f, getHeight() - h, w, h}, {0.f, 0.f}, 0, WHITE
    );

    auto sff = tm->getTexture("sff.png");
    auto sffY = 40.f + ((float)sin(GetTime()) * 30);
    RenderManager::drawTexture("sff.png", {(getWidth() - sff.width) / 2, sffY, static_cast<float>(sff.width), static_cast<float>(sff.height)});

    auto textsize = RenderManager::getTextSize("Sandbox", "boldfont", 35.f);
    RenderManager::drawText("boldfont", "Sandbox", {(getWidth() - textsize.x) / 2, sffY + sff.height * 0.7f}, COL_WHITE, 35.f);

    Scene::draw();
}