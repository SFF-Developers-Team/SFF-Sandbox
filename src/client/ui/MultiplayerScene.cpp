#include <ui/MultiplayerScene.hpp>
#include <ui/JoinServerScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <Game.hpp>
#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <ui/nodes/TextInput.hpp>

MultiplayerScene::MultiplayerScene() : MenuBase() {
    auto const sw = static_cast<float>(GetScreenWidth());
    auto const sh = static_cast<float>(GetScreenHeight());
    auto const btnSize = Vec2f {200.f, 40.f};
    auto const btnPos = Vec2f {sw / 2.f, 460.f};
    auto const game = Game::get();
    auto const padding = 60.f;

    auto usernameLabel = std::make_shared<Text>("Username:", 22.f);
    usernameLabel->setPos({sw / 2, 270.f});
    addChild(usernameLabel);

    auto usernameInput = std::make_shared<TextInput>();
    usernameInput->setPos({sw / 2, 300.f});
    usernameInput->setSize(btnSize);
    usernameInput->setTag("username-input");
    addChild(usernameInput);

    auto hostnameLabel = std::make_shared<Text>("Server IP:", 22.f);
    hostnameLabel->setPos({sw / 2, 350.f});
    addChild(hostnameLabel);

    auto hostnameInput = std::make_shared<TextInput>();
    hostnameInput->setPos({sw / 2, 380.f});
    hostnameInput->setSize(btnSize);
    hostnameInput->setTag("hostname-input");
    addChild(hostnameInput);
    
    auto joinBtn = std::make_shared<Button>("Join", [this, game]() {
        auto usernameInput = std::dynamic_pointer_cast<TextInput>(this->getChild("username-input"));
        auto hostnameInput = std::dynamic_pointer_cast<TextInput>(this->getChild("hostname-input"));

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

    auto backBtn = std::make_shared<Button>("Back", [this]() { this->exit(); });
    backBtn->setX(btnPos.x);
    backBtn->setY(btnPos.y + padding);
    backBtn->setSize(btnSize);
    addChild(backBtn);
}