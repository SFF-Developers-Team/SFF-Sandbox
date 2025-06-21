#include <managers/Debug.hpp>
#include <raylib.h>
#include <GitHash.hpp>
#include <ui/basic/ScrollableContainer.hpp>
#include <ui/basic/Text.hpp>
#include <ui/basic/TextInput.hpp>
#include <managers/RenderManager.hpp>
#include <ui/basic/Scene.hpp>

Debug::Debug() {
    setString(GAME_VERSION, "SFF Sandbox {}-dev ({} {})", GitHash::shortSha1, __DATE__, __TIME__);
}

void Debug::draw() {
    float y = 0.f;
    setString(FPS, "{} FPS", GetFPS());

    for (auto& [id, str] : m_debugList) {
        auto size = RenderManager::getTextSize(str, "font", 20.f);
        RenderManager::drawRect({0.f, y, size.x, size.y}, {0, 0, 0, 128});
        RenderManager::drawText("font", str, {0.f, y}, COL_WHITE, 20.f);
        y += size.y;

        if(!isVisible() && (id != GAME_VERSION || id != FPS)) {
            break;
        }
    }
}


// I don't want to do it now
#if 0
void Debug::allocateConsole() {
    m_debugConsole = std::make_shared<Container>();
    m_debugConsole->setTag("debug-console");
    m_debugConsole->setZOrder(10000);
    m_debugConsole->setAnchor({0.f, 0.f});

    auto border = m_debugConsole->getBorderWidth();
    auto textCont = std::make_shared<ScrollableContainer>();
    textCont->setPos({border * 2, border * 2});
    textCont->setAnchor({0.f, 0.f});
    m_debugConsole->addChild(textCont);

    auto input = std::make_shared<TextInput>("font", "");
    input->setAnchor({0.f, 0.f});
    m_debugConsole->addChild(input);

    auto text = std::make_shared<Text>("font", "", 25.f);
    text->setAlignH(TextAlignmentH::H_LEFT);
    text->setAlignV(TextAlignmentV::V_TOP);
    text->setPos({textCont->getBorderWidth(), textCont->getBorderWidth()});
    text->setAnchor({0.f, 0.f});
    text->setText(m_log);
    text->setWordWrap(true, true);
    textCont->addChild(text);

    m_log.clear();
}

void Debug::update() {
    auto border = m_debugConsole->getBorderWidth();
    m_debugConsole->setPos({0.f, 0.f});
    m_debugConsole->setSize({static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) / 3});
    auto textCont = m_debugConsole->getChild<ScrollableContainer>(0);
    auto input = m_debugConsole->getChild<TextInput>(1);
    auto text = textCont->getChild<Text>(0);

    textCont->setSize({
        m_debugConsole->getWidth() - border * 4, 
        m_debugConsole->getHeight() - border * 5 - input->getHeight()
    });

    text->setWidth(textCont->getWidth());
    input->setPos({border * 2, textCont->getBottomY() + border});
    input->setWidth(textCont->getWidth());

    if(IsKeyPressed(KEY_GRAVE)) {
        auto scene = Scene::get();
        if (scene && !scene->hasChild("debug-console")) {
            scene->addChild(m_debugConsole);
        }

        m_debugConsole->setVisible(!m_debugConsole->isVisible());
    }
}

void Debug::log(std::string const& str) {
    if (m_debugConsole != nullptr) {
        auto cont = m_debugConsole->getChild<Container>(0);
        auto text = cont->getChild<Text>(0);
        text->append(str);
    } else {
        m_log.append(str);
    }
}
#endif