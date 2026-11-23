#include <raylib-cpp.hpp>
#include <ui/basic/Node.hpp>
#include <ui/basic/Container.hpp>
#include <rlgl.h>
#include <raymath.h>
#include <managers/RenderManager.hpp>

Node::Node() : 
    m_bounds(0.f, 0.f, 32.f, 32.f), m_anchor(0.5f, 0.5f), m_scale(1.f, 1.f), m_color(raylib::Color::White()), m_enabled(true), m_visible(true), m_zOrder(0), m_flags(0), m_flipX(false), m_flipY(false), m_rotation(0.f) {}

raylib::Rectangle Node::getWorldBounds() {
    Matrix mat = rlGetMatrixTransform();
    auto pos = raylib::Vector2::Zero().Transform(mat);
    auto scaleX = sqrtf(mat.m0 * mat.m0 + mat.m1 * mat.m1 + mat.m2 * mat.m2);
    auto scaleY = sqrtf(mat.m4 * mat.m4 + mat.m5 * mat.m5 + mat.m6 * mat.m6);

    return {pos.x, pos.y, m_bounds.width * scaleX, m_bounds.height * scaleY};
}

raylib::Vector2 Node::getLocalMousePosition() {
    Matrix mat = rlGetMatrixTransform();
    auto globalPos = raylib::Vector2::Zero().Transform(mat);
    auto mouse = raylib::Mouse::GetPosition();

    auto scaleX = sqrtf(mat.m0 * mat.m0 + mat.m1 * mat.m1 + mat.m2 * mat.m2);
    auto scaleY = sqrtf(mat.m4 * mat.m4 + mat.m5 * mat.m5 + mat.m6 * mat.m6);

    return {(mouse.x - globalPos.x) / scaleX, (mouse.y - globalPos.y) / scaleY};
}

void Node::setZOrder(int z) {
    m_zOrder = z;
    
    if(m_parent != nullptr) {
        m_parent->sortChildsZ();
    }
}

float Node::getGlobalScaleX() {
    Matrix mat = rlGetMatrixTransform();

    return sqrtf(mat.m0 * mat.m0 + mat.m1 * mat.m1 + mat.m2 * mat.m2);
}

float Node::getGlobalScaleY() {
    Matrix mat = rlGetMatrixTransform();

    return sqrtf(mat.m4 * mat.m4 + mat.m5 * mat.m5 + mat.m6 * mat.m6);
}

bool Node::isMouseHover() {
    auto mouse = getLocalMousePosition();
    raylib::Rectangle rect(m_bounds.GetSize());
    
    return rect.CheckCollision(mouse);
}

void Node::draw() {
    if (Debug::get()->isVisible()) {
        RenderManager::drawRectLines(getSize(), raylib::Color::Green(), 1.f);
    }
}