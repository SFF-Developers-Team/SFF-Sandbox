#include <ui/nodes/Layer.hpp>
#include <raylib.h>
#include <rlgl.h>

Layer::Layer() : Container() {
    m_color = {0, 0, 0, 0};
    m_border = 0;
    m_bounds = {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    m_anchor = {0, 0};
}

void Layer::draw() {
    rlPushMatrix();
        rlLoadIdentity();
        Container::draw();
    rlPopMatrix();
}

void Layer::update() {
    rlPushMatrix();
        rlLoadIdentity();
        Container::update();
    rlPopMatrix();
}