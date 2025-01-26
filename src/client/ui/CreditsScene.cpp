#include <ui/CreditsScene.hpp>
#include <RenderManager.hpp>
#include <raylib.h>
CreditsScene::CreditsScene() {
    m_bgColor = COL_SKYBLUE;

}
void CreditsScene::draw() {
    MenuBase::draw();

    DrawRectangleLinesEx({(float)(GetScreenWidth() - 1000) / 2 - 10, 290, 1020, 420}, 10.f, GREEN);
    DrawRectangle((GetScreenWidth() - 1000) / 2, 300, 1000, 400, DARKGREEN);
    auto const sw = static_cast<float>(GetScreenWidth());

    Vector2 offset = {sw / 2 - (picsize.x * devs.size() + 50 * devs.size()) / 2 + 10, 350};
    for (int i = 0; i < devs.size(); i++) {
        auto rm = RenderManager::get();
        auto namewidth = MeasureText(devs[i].name.c_str(), textheight);
        auto rolewidth = MeasureText(devs[i].role.c_str(), textheight);

        rm->drawTile("developers.png", i, {offset.x, offset.y, picsize.x, picsize.y});
        DrawText(devs[i].name.c_str(), offset.x + picsize.x / 2 - namewidth / 2, offset.y + textoffset + 120, textheight, RAYWHITE);
        DrawText(devs[i].role.c_str(), offset.x + picsize.x / 2 - rolewidth / 2, offset.y + textoffset * 2 + textheight + 120, textheight, RAYWHITE);
        offset.x += picsize.x + 50;
    }   
    drawButton("Special Thanks", {(float)(GetScreenWidth() - 200) / 2, 600, 200, 40}, [&]() {

    });
    drawButton("Back", {(float)(GetScreenWidth() - 200) / 2, 650, 200, 40}, [&]() {
        Game::get()->popScene();
    });
    // if(GuiButton((Rectangle){ (float)(GetScreenWidth() - 200) / 2,   650, 200, 40 }, "Back") || IsKeyPressed(KEY_Q)) {
    //     auto game = Game::get();
    //     game->pushScene(std::make_shared<MainScene>());
    //     count = 0;
    // }
}