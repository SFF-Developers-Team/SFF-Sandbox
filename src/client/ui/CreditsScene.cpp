#include <ui/CreditsScene.hpp>
#include <StyleManager.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <raylib.h>

CreditsScene::CreditsScene() {
    
}

void CreditsScene::draw() {
    MenuBase::draw();

    auto sm = StyleManager::get();
    auto rm = TextureManager::get();
    auto rdm = RenderManager::get();
    auto const consize = Vec2f {700.f, 300.f}; 
    auto const picsize = Vec2f {128.f, 128.f};
    auto const picsoffset = 30.f;
    auto const fcolor = sm->getValue<Col4u>(FIRST_COLOR_NORMAL);
    auto const scolor = sm->getValue<Col4u>(SECOND_COLOR_NORMAL);
    auto const border = sm->getValue<float>(BORDER_WIDTH);
    auto const boldfont = rm->getFont("boldfont");
    auto const font = rm->getFont("font");

    // clang-format off
    std::vector<Developer> const devs = {
        {"del", "Artist"},
        {"e2e4", "Artist"},
        {"InviseDivine", "Programmer"},
        {"Kolyah35", "Programmer"},
        {"dogotrigger", "Programmer"},
    };
    // clang-format on

    DrawRectangle((getWidth() - consize.x) / 2, (getHeight() - consize.y) / 2, consize.x, consize.y, fcolor.to<Color>());
    DrawRectangleLinesEx({(getWidth() - consize.x) / 2, (getHeight() - consize.y) / 2, consize.x, consize.y}, border, scolor.to<Color>());

    auto start = Vec2f {
        .x = getWidth() / 2 - picsize.x * devs.size() + picsoffset * devs.size(),
        .y = getHeight() / 2 - picsize.y + 40.f
    };

    for(auto i = 0; i < devs.size(); i++) {
        rdm->drawTile("developers.png", i, {start.x, start.y, picsize.x, picsize.y});
        
        auto namesize = MeasureTextEx(boldfont, devs[i].name.c_str(), 16.f, 1.f);
        auto rolesize = MeasureTextEx(font, devs[i].role.c_str(), 16.f, 1.f);

        DrawTextEx(boldfont, devs[i].name.c_str(), {start.x + picsize.x / 2 - namesize.x / 2, start.y + picsize.y + 4.f}, 40.f, 1.f, RAYWHITE);
        DrawTextEx(font, devs[i].role.c_str(), {start.x + picsize.x / 2 - namesize.x / 2, start.y + picsize.y + 20.f}, 40.f, 1.f, RAYWHITE);

        start.x += picsize.x + picsoffset;
    }
}