#include <TextureManager.hpp>
#include <RenderManager.hpp>
#include <ui/MenuBase.hpp>
#include <TileMap.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <array>

auto const bgwidth = 11;
auto const bgheight = 6;
RenderTexture2D m_bgRender;

MenuBase::MenuBase() : Scene() {
    m_color = COL_SKYBLUE;
    
    if(m_bgRender.id == 0) {
        auto game = Game::get();

        m_bgRender = LoadRenderTexture(bgwidth * 16, bgheight * 16);
    
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

                    RenderManager::drawTile("blocks.png", blocks[i] - 1, {x * 16, y * 16, 16, 16}, COL_WHITE);
                }
            }

            auto player = TextureManager::get()->getTexture("player.png");

            RenderManager::renderPlayerTexture({70, 14}, "player.png", {16.f, 22.f}, 15, RIGHT);
        EndTextureMode();
    }
}

void MenuBase::update() {
    Scene::update();

    auto game = Game::get();
    
    for(auto& child : m_childs) {
        if(child->getTag().find("center") != std::string::npos) {
            child->setPos({getWidth() / 2, getHeight() / 2});
            child->setScale(game->getGuiScale());
        }
    }
}

void MenuBase::draw() {
    auto tm = TextureManager::get();
    auto w = static_cast<float>(m_bgRender.texture.width) * 2.5f * getGlobalScaleX();
    auto h = static_cast<float>(m_bgRender.texture.height) * 2.5f * getGlobalScaleY();

    DrawTexturePro(
        m_bgRender.texture, 
        {0, 0, (float)m_bgRender.texture.width, (float)-m_bgRender.texture.height},
        {-32.f, getHeight() - h, w, h}, {0.f, 0.f}, 0, WHITE
    );

    // auto sff = tm->getTexture("sff.png");
    // auto sffY = 40.f + ((float)sin(GetTime()) * 30);
    // RenderManager::drawTexture("sff.png", {(getWidth() - sff.width) / 2, sffY, static_cast<float>(sff.width), static_cast<float>(sff.height)});

    // auto textsize = RenderManager::getTextSize("Sandbox", "boldfont", 35.f);
    // RenderManager::drawText("boldfont", "Sandbox", {(getWidth() - textsize.x) / 2, sffY + sff.height * 0.7f}, COL_WHITE, 35.f);

    Scene::draw();
}