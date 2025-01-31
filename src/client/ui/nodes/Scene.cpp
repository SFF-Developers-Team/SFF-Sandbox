#include <ui/nodes/Scene.hpp>
#include <rlgl.h>

Scene::Scene() : Container() {
    m_border = 0.f;
    m_bounds = {GetScreenWidth() / 2.f, GetScreenHeight() / 2.f, 1280.f, 720.f};
}

void Scene::update() {
    if(IsKeyPressed(KEY_ESCAPE) || m_destroy) {
        return Game::get()->popScene();
    }

    Container::update();
}

void Scene::destroy() {
    m_destroy = true;
}

void Scene::draw() {
    for(auto& node : m_childs) {
        rlPushMatrix();
            rlTranslatef(node->getX(), node->getY(), 0.f);
            rlTranslatef(-(node->getAnchorX() * node->getWidth()), -(node->getAnchorY() * node->getHeight()), 0.f);
            
            node->draw();
        rlPopMatrix();
    }
}