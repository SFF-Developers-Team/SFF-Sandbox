#include <raylib-cpp.hpp>
#include <scenes/MenuBase.hpp>
#include <managers/ResourceManager.hpp>
#include <managers/RenderManager.hpp>
#include <Tilemap.hpp>
#include <Types.hpp>
#include <Game.hpp>
#include <array>
#include <cmath>

auto const bgwidth = 11;
auto const bgheight = 10;
raylib::RenderTexture m_bgRender;

MenuBase::MenuBase() : Scene() {
    m_color = raylib::Color::SkyBlue();
    
    if(!m_bgRender.IsValid()) {
        auto& rm = ResourceManager::Get();
        auto& tilemap = rm.GetTilemap("tiles.png");

        m_bgRender.Load(bgwidth * 16, bgheight * 16);
    
        // 0 - AIR; 1 - GRASS; 2 - DIRT; 3 - STONE
        std::array<uint8_t, bgwidth * bgheight> tiles = {
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

        m_bgRender.BeginMode();
            Game::get()->getWindow().ClearBackground({0, 0, 0, 0});

            for(int i = 0; i < tiles.size(); i++) {
                if(tiles[i] != 0) {
                    auto color = (tiles[i] == 11) ? raylib::Color::Green() : raylib::Color::White();
                    auto x = static_cast<float>(i % bgwidth);
                    auto y = static_cast<float>(i / bgwidth);

                    tilemap.DrawTile(tiles[i] - 1, raylib::Rectangle(x * 16, y * 16, 16, 16), color);
                }
            }

            RenderManager::renderPlayerTexture({70, 78}, "player.png", {16.f, 22.f}, 15, RIGHT);
        m_bgRender.EndMode();
    }
}

void MenuBase::draw() {
    auto scale = Game::get()->getGuiScale();
    auto w = static_cast<float>(m_bgRender.texture.width) * 2.5f * scale;
    auto h = static_cast<float>(m_bgRender.texture.height) * 2.5f * scale;

    m_bgRender.GetTexture().Draw(
        raylib::Rectangle {0.f, 0.f, float(m_bgRender.texture.width), float(-m_bgRender.texture.height)},
        raylib::Rectangle {-32.f, getHeight() - h, w, h}
    );

    auto& rm = ResourceManager::Get();
    auto& sff = rm.GetTexture("sff.png");
    auto sffY = 40.f + std::sin(raylib::Window::GetTime()) * 30;

    sff.Draw(raylib::Vector2((getWidth() - sff.width) / 2, sffY));
    
    auto& font = rm.GetFont("boldfont");
    font.DrawText("Sandbox", Vector2(getWidth(), sffY), Vector2(0.5f, 0.7f), 0.f, 35.f, 1.f, raylib::Color::White());

    Scene::draw();
}
void MenuBase::releaseBackground() {
    m_bgRender.Unload();
}
