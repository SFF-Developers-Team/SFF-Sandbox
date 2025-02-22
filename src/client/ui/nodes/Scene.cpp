#include <ui/nodes/Scene.hpp>
#include <rlgl.h>
#include <MiniFunction.hpp>

Scene::Scene() : Container() {
    setScale(1.f);
    setBorderWidth(0.f);
    setFrameVisible(false);
    setPos({0.f, 0.f});
    setAnchor({0.f, 0.f});
    setColor({0, 0, 0, 0});

    onShow();
}

void Scene::keyBackClicked() {
    if(m_keyBack) {
        m_destroy = true;
    }
}

void Scene::update() {
#ifndef PLATFORM_ANDROID
    if(IsWindowResized()) {
        onShow();
    }
#endif

    Container::update();
}

void Scene::onShow() {
    auto scrW = static_cast<float>(GetScreenWidth());
    auto scrH = static_cast<float>(GetScreenHeight());

    setSize({scrW, scrH});
}