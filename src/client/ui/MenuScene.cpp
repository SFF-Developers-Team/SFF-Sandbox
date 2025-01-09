#include <ui/MenuScene.hpp>
#include <ui/PlayScene.hpp>
#include <ui/MultiplayerScene.hpp>
#include <ui/CreditsScene.hpp>
#include <raygui.h>

void MenuScene::draw() {
    // DrawTexture(tm->getTexture("sff.png"), (GetScreenWidth() - tm->getTexture("sff.png").width) / 2, pos, WHITE);
    // DrawText("Sandbox", (GetScreenWidth() - 35) / 2 - (tm->getTexture("sff.png").width - 35) / 2 - 5, pos + 35 + tm->getTexture("sff.png").width / 2, 35, RAYWHITE);

    // if (GuiButton((Rectangle){(float)(GetScreenWidth() - 300) / 2, 300, 300, 40 }, "Play")) {
    //     auto game = Game::get();
    //     if(!game->getWorld()->load()) { 
    //         game->getWorld()->setGenerator(std::make_shared<WorldGenNormal>(game->getWorld(), 1));
    //         game->getWorld()->generate();
    //     }

    //     game->getWorld()->addPlayer(1, game->getPlayer());
    //     game->pushScene(std::make_shared<PlayScene>());
    // }

    // if (GuiButton((Rectangle){ (float)(GetScreenWidth() - 300) / 2, 360, 300, 40 }, "Multiplayer")) {
    //     auto game = Game::get();

    //     game->pushScene(std::make_shared<MultiplayerScene>());
    // }

    // if (GuiButton((Rectangle){ (float)(GetScreenWidth() - 300) / 2, 420, 300, 40 }, "Credits")) {
    //     auto game = Game::get();

    //     game->pushScene(std::make_shared<CreditsScene>());
    // }

    // if (GuiButton((Rectangle){ (float)(GetScreenWidth() - 300) / 2, 480, 300, 40 }, "Options")) {
    //     auto game = Game::get();

    //     // game->pushScene(std::make_shared<Settings>());
    // }
}