#include <ui/TestScene.hpp>
#include <ui/nodes/SpriteNode.hpp>
#include <ui/nodes/Slider.hpp>

TestScene::TestScene() : Scene() {
    m_color = COL_SKYBLUE;

    auto texture = std::make_shared<SpriteNode>("raylib.png");
    texture->setPos(getSize() / 2);
    texture->setTag("tex");
    addChild(texture);

    auto xSlider = std::make_shared<Slider<float>>("Position X ", 0.f, getWidth(), [texture] (auto, float x) { texture->setX(x); });
    xSlider->setPos({getWidth() - xSlider->getWidth(), 20.f});
    addChild(xSlider);

    auto ySlider = std::make_shared<Slider<float>>("Position Y ", 0.f, getHeight(), [texture] (auto, float y) { texture->setY(y); });
    ySlider->setPos({xSlider->getX(), xSlider->getY() + xSlider->getHeight()});
    addChild(ySlider);

    auto wSlider = std::make_shared<Slider<float>>("Width ", 0.f, 512.f, [texture] (auto, float w) { texture->setWidth(w); });
    wSlider->setPos({xSlider->getX(), ySlider->getY() + ySlider->getHeight()});
    addChild(wSlider);

    auto hSlider = std::make_shared<Slider<float>>("Height ", 0.f, 512.f, [texture] (auto, float h) { texture->setHeight(h); });
    hSlider->setPos({xSlider->getX(), wSlider->getY() + wSlider->getHeight()});
    addChild(hSlider);

    auto anchorXSlider = std::make_shared<Slider<float>>("Anchor X ", 0.f, 1.f, [texture] (auto, float ax) { texture->setAnchorX(ax); });
    anchorXSlider->setPos({xSlider->getX(), hSlider->getY() + hSlider->getHeight()});
    addChild(anchorXSlider);

    auto anchorYSlider = std::make_shared<Slider<float>>("Anchor Y ", 0.f, 1.f, [texture] (auto, float ay) { texture->setAnchorY(ay); });
    anchorYSlider->setPos({xSlider->getX(), anchorXSlider->getY() + anchorXSlider->getHeight()});
    addChild(anchorYSlider);

    auto scaleXSlider = std::make_shared<Slider<float>>("Scale X ", 0.f, 5.f, [texture] (auto, float scalex) { texture->setScaleX(scalex); });
    scaleXSlider->setPos({xSlider->getX(), anchorYSlider->getY() + anchorYSlider->getHeight()});
    addChild(scaleXSlider);

    auto scaleYSlider = std::make_shared<Slider<float>>("Scale Y ", 0.f, 5.f, [texture] (auto, float scaley) { texture->setScaleY(scaley); });
    scaleYSlider->setPos({xSlider->getX(), scaleXSlider->getY() + scaleXSlider->getHeight()});
    addChild(scaleYSlider);
}

void TestScene::draw() {
    Scene::draw();

    auto texture = getChild<SpriteNode>("tex");
    auto pos = texture->getPos();
    auto size = texture->getScaledSize();
    
    RenderManager::drawRectLines({pos.x - size.x * texture->getAnchorX(), pos.y - size.y * texture->getAnchorY(), size.x, size.y}, COL_GREEN, 1.f);
    RenderManager::drawRect({pos.x, pos.y, 2.f, 2.f}, COL_RED);
}