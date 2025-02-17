#include <ui/nodes/Scene.hpp>
#include <rlgl.h>

Scene::Scene() : Container() {
    setScale(1.f);
    setBorderWidth(0.f);
    onShow();
}

void Scene::update() {
    if(m_keyBack && IsKeyPressed(KEY_ESCAPE)) {
        m_destroy = true;
    }

    if(IsWindowResized()) {
        onShow();
    }

    Container::update();
}

void Scene::onShow() {
    auto scrW = static_cast<float>(GetScreenWidth());
    auto scrH = static_cast<float>(GetScreenHeight());

    setPos({scrW / 2.f, scrH / 2.f});
    setSize({scrW, scrH});
}

void Scene::destroy() {
    m_destroy = true;
}

bool Scene::shouldDestroy() {
    return m_destroy;
}

void Scene::draw() {
    for(auto& node : m_childs) {
        if(node->isVisible()) {
            rlPushMatrix();
                rlTranslatef(node->getX(), node->getY(), 0.f);
                rlTranslatef(-(node->getAnchorX() * node->getScaleX() * node->getWidth()), -(node->getAnchorY() * node->getScaleY() * node->getHeight()), 0.f);
                rlScalef(node->getScaleX(), node->getScaleY(), 0.f);
                
                node->draw();
            rlPopMatrix();
        }
    }
}