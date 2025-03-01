#include <ui/basic/Node.hpp>
#include <ui/basic/Container.hpp>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <RenderManager.hpp>

Node::Node() : 
    m_bounds({0.f, 0.f, 32.f, 32.f}), m_anchor({0.5f, 0.5f}), m_scale({1.f, 1.f}), m_color(COL_WHITE), m_enabled(true), m_visible(true), m_zOrder(0), m_flags(0), m_flipX(false), m_flipY(false), m_rotation(0.f) {}

Rectf Node::getWorldBounds() {
    Matrix mat = rlGetMatrixTransform();
    auto pos = Vector2Transform({0, 0}, mat);
    auto scaleX = sqrtf(mat.m0 * mat.m0 + mat.m1 * mat.m1 + mat.m2 * mat.m2);
    auto scaleY = sqrtf(mat.m4 * mat.m4 + mat.m5 * mat.m5 + mat.m6 * mat.m6);

    return {pos.x, pos.y, m_bounds.width * scaleX, m_bounds.height * scaleY};
}

Vec2f Node::getLocalMousePosition() {
    Matrix mat = rlGetMatrixTransform();
    auto globalPos = Vector2Transform({0.f, 0.f}, mat);
    auto mouse = GetMousePosition();

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
    Vec2f mouse = getLocalMousePosition();
    Rectf rect = {0.f, 0.f, m_bounds.width, m_bounds.height};
    
    return rect.contains(mouse);
}

void Node::draw() {
    if (Debug::get()->isVisible()) {
        RenderManager::drawRectLines({0.f, 0.f, getWidth(), getHeight()}, COL_GREEN, 1.f);
    }
}