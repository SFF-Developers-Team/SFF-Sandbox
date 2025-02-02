#include <ui/nodes/Scene.hpp>
#include <rlgl.h>

Scene::Scene() : Container() {
    auto scrW = static_cast<float>(GetScreenWidth());
    auto scrH = static_cast<float>(GetScreenHeight());

    m_bounds = {scrW / 2.f, scrH / 2.f, scrW, scrH};
    m_border = 0.f;
}

void Scene::update() {
    if(IsKeyPressed(KEY_ESCAPE)) {
        m_destroy = true;
    }

    if(IsWindowResized()) {
        auto scrW = static_cast<float>(GetScreenWidth());
        auto scrH = static_cast<float>(GetScreenHeight());

        m_bounds = {scrW / 2.f, scrH / 2.f, scrW, scrH};
    }

    Container::update();
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
                rlTranslatef(-(node->getAnchorX() * node->getWidth()), -(node->getAnchorY() * node->getHeight()), 0.f);
                
                node->draw();
            rlPopMatrix();
        }
    }
}