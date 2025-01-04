#include <Menu.hpp>
#include <stdio.h>
PlayScene::PlayScene() {
    m_blocksMap = std::make_shared<TileMap>("assets/blocks.png", Vector2 {16, 16});
    m_timer = std::make_shared<Timer>(60);
    m_world = std::make_shared<World>("world1");
    m_player = std::make_shared<Player>(m_world);
    // m_particleManager = std::make_shared<ParticleManager>(m_world, m_player);
    m_renderManager = std::make_shared<RenderManager>(m_world, m_player);
}
void PlayScene::draw() {

    BeginMode2D(m_player->getCamera());
        m_renderManager->renderWorld();
    EndMode2D();

    auto selectedBlock = m_player->getSelectedBlock();
        
    if(selectedBlock) {
        m_renderManager->renderUIBlock(GetScreenWidth() - 42.f, 10.f, 32.f, 32.f, selectedBlock);
    }

    auto dbg = Debug::get();
    m_blocksMap->drawTile(0, {0, 0}, RAYWHITE, 0);
    if(dbg->isVisible()){
        dbg->draw();
    } else {
        DrawText(std::format("SFF Sandbox {}-dev ({} {})", GitHash::shortSha1, __DATE__, __TIME__).c_str(), 5, 5, 20, WHITE);
        DrawText(std::format("{} FPS", GetFPS()).c_str(), 5, 30, 20, WHITE);
    }
}
void PlayScene::update() {
    m_timer->advanceTime();

    for (uint32_t i = 0; i < m_timer->getTicks(); i++) {
        m_world->onTick();
    }

    m_player->update();

    if(IsKeyPressed(KEY_F3)) {
        auto dbg = Debug::get();
        dbg->setVisible(!dbg->isVisible());
    }

    if(IsKeyPressed(KEY_F6)) {
        m_world->save();
    }

    if(IsKeyPressed(KEY_F1)) {
        CloseWindow();
    }
}
void CreditsScene::draw() {
    static bool isLoaded;
    static int count;

    DrawText(TextFormat("%f", offset.x), 0, 0, 25, MAROON);
    DrawText(TextFormat("%f", offset.y), 0, 25, 25, MAROON);

    DrawRectangle((GetScreenWidth() - 1000) / 2, 300, 1000, 400, {49, 132, 124, 255});
    DrawRectangleRec(container, {34, 117, 109, 255});

    for(auto& dev : devs) {
        printf("X: %f \n", offset.x);
        printf("Y: %f \n", offset.y);
        
        auto namewidth = MeasureText(dev.name.c_str(), textheight);
        auto rolewidth = MeasureText(dev.role.c_str(), textheight);

        DrawTexturePro(dev.texture, {0, 0, (float)dev.texture.width, (float)dev.texture.height}, {offset.x, offset.y, picsize.x, picsize.y}, {0, 0}, 0, RAYWHITE);
        DrawText(dev.name.c_str(), offset.x + picsize.x / 2 - namewidth / 2, offset.y + textoffset + 20, textheight, RAYWHITE);
        DrawText(dev.role.c_str(), offset.x + picsize.x / 2 - rolewidth / 2, offset.y + textoffset * 2 + textheight + 20, textheight, RAYWHITE);
        if(count < devs.size()) {
            offset.x += picsize.x + devsoffset;
        }
        count++;
    }

    if(GuiButton((Rectangle){ (float)(GetScreenWidth() - 200) / 2,   650, 200, 40 }, "Back") || IsKeyPressed(KEY_Q)) {
        auto game = Game::get();
        game->pushScene(std::make_shared<MainScene>());
        count = 0;
    }
}
void CreditsScene::update() {
    
}
MultiplayerScene::MultiplayerScene() {
    m_ip = (char*)MemAlloc(64);
    m_nickname = (char*)MemAlloc(32);
}
MultiplayerScene::~MultiplayerScene() {
    free(m_ip);
    free(m_nickname);
}
void MultiplayerScene::update() {
    Vector2 m_mousePos = GetMousePosition();

    m_isNick = CheckCollisionPointRec(m_mousePos, m_nickBox);
    m_isIP = CheckCollisionPointRec(m_mousePos, m_ipBox);
}
void MultiplayerScene::draw() {
    DrawText("Nickname:", (float)(GetScreenWidth() - 200) / 2 - 125, 300, 25, RAYWHITE);
    DrawText("IP:", (float)(GetScreenWidth() - 200) / 2 - 50, 360, 25, RAYWHITE);

    GuiTextBox(m_nickBox, m_nickname, 32, m_isNick);
    GuiTextBox(m_ipBox, m_ip, 64, m_isIP);

    if(GuiButton((Rectangle){ (float)(GetScreenWidth() - 200) / 2, 420, 200, 40 }, "Join")) {
        // ..
    };
    if(GuiButton((Rectangle){ (float)(GetScreenWidth() - 200) / 2, 480, 200, 40 }, "Back") || IsKeyPressed(KEY_Q)) {
        auto game = Game::get();
        game->pushScene(std::make_shared<MainScene>());
    }
}

void MainScene::draw() {

        if (GuiButton((Rectangle){ (float)(GetScreenWidth() - 300) / 2, 300, 300, 40 }, "Play")) {
            auto game = Game::get();

            game->pushScene(std::make_shared<PlayScene>());
        }      
        if (GuiButton((Rectangle){ (float)(GetScreenWidth() - 300) / 2, 360, 300, 40 }, "Multiplayer")) {
            auto game = Game::get();

            game->pushScene(std::make_shared<MultiplayerScene>());
        }       
        if (GuiButton((Rectangle){ (float)(GetScreenWidth() - 300) / 2, 420, 300, 40 }, "Credits")) {
            auto game = Game::get();

            game->pushScene(std::make_shared<CreditsScene>());
        }    
        if (GuiButton((Rectangle){ (float)(GetScreenWidth() - 300) / 2, 480, 300, 40 }, "Options")) {
            auto game = Game::get();

            // game->pushScene(std::make_shared<Settings>());
        }


}