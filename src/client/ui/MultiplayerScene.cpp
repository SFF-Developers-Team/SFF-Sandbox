#include <ui/MultiplayerScene.hpp>
#include <ui/JoinServerScene.hpp>
#include <ui/MainMenuScene.hpp>
#include <Game.hpp>
#include <ui/nodes/Button.hpp>
#include <ui/nodes/Text.hpp>
#include <ui/nodes/TextInput.hpp>
#include <StyleManager.hpp>

MultiplayerScene::MultiplayerScene() : MenuBase() {
    // TODO: Optimize it
    auto const game = Game::get();

    auto elementSize = StyleManager::get()->getValue<Vec2f>(DEFAULT_ELEMENT_SIZE);
    auto container = std::make_shared<Container>();
    container->setTag("center-buttons");
    container->setWidth(elementSize.x);
    container->setColor({0, 0, 0, 0});
    container->setBorderWidth(0.f);
    container->setFlag(FLAG_ALWAYS_CENTER, true);
    container->setFlag(FLAG_GUI_SCALE, true);
    addChild(container);

    auto usernameInput = std::make_shared<TextInput>("font", "Username");
    usernameInput->setPos({container->getWidth() / 2, usernameInput->getHeight() / 2.f});
    usernameInput->setTag("username-input");
    container->addChild(usernameInput);

    auto hostnameInput = std::make_shared<TextInput>("font", "Server IP");
    hostnameInput->setX(usernameInput->getX());
    hostnameInput->setY(usernameInput->getY() + usernameInput->getHeight() + usernameInput->getBorderWidth());
    hostnameInput->setTag("hostname-input");
    hostnameInput->setAllowedChars(URL_ALLOWED_CHARS);
    container->addChild(hostnameInput);
    
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

    joinBtn->setX(hostnameInput->getX());
    joinBtn->setY(hostnameInput->getY() + hostnameInput->getHeight() * 2.f + hostnameInput->getBorderWidth());
    container->addChild(joinBtn);

    auto backBtn = std::make_shared<Button>("Back", [this](Button*) { destroy(); });
    backBtn->setX(joinBtn->getX());
    backBtn->setY(joinBtn->getY() + joinBtn->getHeight() + joinBtn->getBorderWidth());
    container->addChild(backBtn);

    container->setHeight(100.f);
}