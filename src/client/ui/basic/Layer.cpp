#include <raylib-cpp.hpp>
#include <ui/basic/Layer.hpp>
#include <rlgl.h>

Layer::Layer() : Container() {
    m_color = raylib::Color::Blank();
    m_border = 0;
    m_bounds = raylib::Window::GetSize();
    m_anchor = raylib::Vector2::Zero();
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