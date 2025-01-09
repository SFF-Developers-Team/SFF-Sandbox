#include <CreditsScene.hpp>
#include <raylib.h>

void CreditsScene::draw() {
    // DrawRectangle((GetScreenWidth() - 1000) / 2, 300, 1000, 400, {49, 132, 124, 255});
    // DrawRectangleRec(container, {34, 117, 109, 255});

    // for(auto& dev : devs) {
    //     printf("X: %f \n", offset.x);
    //     printf("Y: %f \n", offset.y);
        
    //     auto namewidth = MeasureText(dev.name.c_str(), textheight);
    //     auto rolewidth = MeasureText(dev.role.c_str(), textheight);

    //     DrawTexturePro(dev.texture, {0, 0, (float)dev.texture.width, (float)dev.texture.height}, {offset.x, offset.y, picsize.x, picsize.y}, {0, 0}, 0, RAYWHITE);
    //     DrawText(dev.name.c_str(), offset.x + picsize.x / 2 - namewidth / 2, offset.y + textoffset + 20, textheight, RAYWHITE);
    //     DrawText(dev.role.c_str(), offset.x + picsize.x / 2 - rolewidth / 2, offset.y + textoffset * 2 + textheight + 20, textheight, RAYWHITE);
    //     if(count < devs.size()) {
    //         offset.x += picsize.x + devsoffset;
    //     }
    //     count++;
    // }

    // if(GuiButton((Rectangle){ (float)(GetScreenWidth() - 200) / 2,   650, 200, 40 }, "Back") || IsKeyPressed(KEY_Q)) {
    //     auto game = Game::get();
    //     game->pushScene(std::make_shared<MainScene>());
    //     count = 0;
    // }
}