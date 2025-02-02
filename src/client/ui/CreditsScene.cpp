#include <ui/CreditsScene.hpp>
#include <StyleManager.hpp>
#include <RenderManager.hpp>
#include <TextureManager.hpp>
#include <ui/nodes/Tile.hpp>
#include <ui/nodes/SpriteNode.hpp>
#include <ui/nodes/List.hpp>
#include <Licenses.hpp>

CreditsScene::CreditsScene() {
    // clang-format off
    std::vector<Developer> const devs = {
        {"Del", "Artist"},
        {"e2e4", "Artist"},
        {"InviseDivine", "Programmer"},
        {"Kolyah35", "Programmer"},
        {"dogotrigger", "Programmer"},
    };
    // clang-format on

    auto container = std::make_shared<Container>();
    container->setSize({1000, 500});
    container->setPos({getWidth() / 2, getHeight() / 2 + 100.f});
    addChild(container);

    m_developersBox = std::make_shared<Container>();
    m_licensesBox = std::make_shared<Container>();

    // developers container
    {
        m_developersBox->setBorderWidth(0.f);
        m_developersBox->setAnchor({0.5f, 0.f});
        m_developersBox->setColor({0, 0, 0, 0});
        m_developersBox->setSize({container->getWidth(), container->getHeight() - 150});
        m_developersBox->setPos({container->getWidth() / 2, 0.f});
        container->addChild(m_developersBox);

        for(auto i = 0; i < devs.size(); i++) {
            auto tile = std::make_shared<Tile>("developers.png", i);
            tile->setSize({128.f, 128.f});
            m_developersBox->addChild(tile);
        }

        m_developersBox->alignItemsHorizontal(64.f);

        for(auto i = 0; i < devs.size(); i++) {
            auto child = m_developersBox->getChild(i);

            auto name = std::make_shared<Text>("boldfont", devs[i].name, 40.f);
            name->setPos({child->getX(), child->getY() + 84.f});
            name->setSize({child->getWidth(), 40.f});
            m_developersBox->addChild(name);

            auto role = std::make_shared<Text>("font", devs[i].role, 40.f);
            role->setPos({child->getX(), name->getY() + 30.f});
            role->setSize({child->getWidth(), 40.f});
            m_developersBox->addChild(role);
        }

        auto title = std::make_shared<Text>("boldfont", "Developers", 40.f);
        title->setPos({m_developersBox->getWidth() / 2, 30.f});
        m_developersBox->addChild(title);
    }

    // licenses container
    {
        m_licensesBox->setBorderWidth(0.f);
        m_licensesBox->setAnchor({0.5f, 0.f});
        m_licensesBox->setColor({0, 0, 0, 0});
        m_licensesBox->setSize({container->getWidth(), container->getHeight() - 150});
        m_licensesBox->setPos({container->getWidth() / 2, 0.f});
        m_licensesBox->setVisible(false);
        m_licensesBox->setEnabled(false);
        container->addChild(m_licensesBox);


        auto libsList = std::make_shared<List>(std::vector<std::string>({"enet", "GitHash", "perlin-noise", "raylib", "toml", "zlib"}), [this](int i) {
            m_licenseText->setText(licensesText[i]);
            m_licenseText->setWordWrap(true, true); // for auto-height
            m_licensesBox->resetScroll();
        });

        auto const borderw = libsList->getBorderWidth();

        libsList->setSize({300.f, m_licensesBox->getHeight() - borderw * 2});
        libsList->setPos({libsList->getWidth() / 2 + borderw * 2, m_licensesBox->getHeight() / 2 + borderw});
        m_licensesBox->addChild(libsList);

        m_textBox = std::make_shared<Container>();
        m_textBox->setAnchor({0.f, 0.5f});
        m_textBox->setPos({libsList->getX() + libsList->getWidth() * libsList->getAnchorX() + borderw, libsList->getY()});
        m_textBox->setSize({m_licensesBox->getWidth() - libsList->getWidth() - borderw * 5, libsList->getHeight()});
        m_textBox->setScrollable(true);
        m_licensesBox->addChild(m_textBox);

        m_licenseText = std::make_shared<Text>("font", licensesText[0], 20.f);
        m_licenseText->setAnchor({0.5f, 0.f});
        m_licenseText->setPos({m_textBox->getWidth() / 2, 0.f});
        m_licenseText->setSize({m_textBox->getWidth() - m_textBox->getBorderWidth() * 2, 0.f});
        m_licenseText->setAlignH(TextAlignmentH::H_LEFT);
        m_licenseText->setAlignV(TextAlignmentV::V_TOP);
        m_licenseText->setWordWrap(true, true);
        m_textBox->addChild(m_licenseText);
    }

    auto backBtn = std::make_shared<Button>("Back", [this](Button*) { destroy(); });
    backBtn->setPos({container->getWidth() / 2, container->getHeight() - 40});
    container->addChild(backBtn);

    auto licensesBtn = std::make_shared<Button>("Licenses", [this](Button* sender) {
        bool licenses = m_licensesBox->isVisible();

        sender->setText(licenses ? "Licenses" : "Developers");
        m_licensesBox->setVisible(!licenses);
        m_licensesBox->setEnabled(!licenses);
        m_developersBox->setVisible(licenses);
        m_developersBox->setEnabled(licenses);
    });

    licensesBtn->setPos({container->getWidth() / 2, backBtn->getY() - 60.f});
    container->addChild(licensesBtn);

    auto raylib = std::make_shared<SpriteNode>("raylib");
    raylib->setSize({128.f, 128.f});
    raylib->setAnchor({0.f, 1.f});
    raylib->setPos({container->getWidth() * container->getAnchorX() + 5.f, container->getHeight() * container->getAnchorY() - 5.f});
    container->addChild(raylib);
}