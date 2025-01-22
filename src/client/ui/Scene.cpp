#include <ui/Scene.hpp>

Scene::Scene() {}

Col4u Scene::getColor() {
    return m_bgColor;
}

void Scene::setColor(Col4u color) {
    m_bgColor = color;
}

void Scene::update() {
    if(IsKeyPressed(KEY_ESCAPE) || m_shouldExit) {
        return Game::get()->popScene();
    }

    for(auto& node : m_childs) {
        node->update();
    }
}

void Scene::draw() {
    for(auto& node : m_childs) {
        node->draw();
    }
}

void Scene::exit() {
    m_shouldExit = true;
}