#include <ui/MultiplayerScene.hpp>
#include <ui/JoinServerScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <Game.hpp>
#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <ui/nodes/TextInput.hpp>

MultiplayerScene::MultiplayerScene() : MenuBase() {
    auto const btnSize = Vec2f {300.f, 40.f};
    auto const inpSize = Vec2f {300.f, 50.f};
    auto const btnPos = Vec2f {getWidth() / 2.f, 460.f};
    auto const game = Game::get();
    auto const padding = 60.f;

    auto usernameInput = std::make_shared<TextInput>("font", "Username");
    usernameInput->setPos({getWidth() / 2, 300.f});
    usernameInput->setSize(inpSize);
    usernameInput->setTag("username-input");
    addChild(usernameInput);


    auto hostnameInput = std::make_shared<TextInput>("font", "Server IP");
    hostnameInput->setPos({getWidth() / 2, 380.f});
    hostnameInput->setSize(inpSize);
    hostnameInput->setTag("hostname-input");
    addChild(hostnameInput);
    
    auto joinBtn = std::make_shared<Button>("Join", [this, game](Button*) {
        auto usernameInput = this->getChild<TextInput>("username-input");
        auto hostnameInput = this->getChild<TextInput>("hostname-input");

        auto username = usernameInput->getText();
        auto hostname = hostnameInput->getText();

        if(username.size() > 3 && hostname.size() > 2) {
            auto colon = hostname.find(':');
            auto ip = hostname.substr(0, colon);
            uint16_t port = (colon != std::string::npos ? std::stoi(hostname.substr(colon + 1)) : 7777);
            
            game->setUsername(username);
            game->pushScene(std::make_shared<JoinServerScene>(ip, port));
        }
    });

    joinBtn->setPos(btnPos);
    joinBtn->setSize(btnSize);
    addChild(joinBtn);

    auto backBtn = std::make_shared<Button>("Back", [this](Button*) { destroy(); });
    backBtn->setX(btnPos.x);
    backBtn->setY(btnPos.y + padding);
    backBtn->setSize(btnSize);
    addChild(backBtn);
}