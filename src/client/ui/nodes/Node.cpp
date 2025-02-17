#include <ui/nodes/Node.hpp>
#include <ui/nodes/Container.hpp>
#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

Node::Node() : m_bounds({0.f, 0.f, 32.f, 32.f}), m_anchor({0.5f, 0.5f}), m_scale({1.f, 1.f}), m_color(COL_WHITE), m_enabled(true), m_visible(true), m_zOrder(0) {}

Rectf Node::getWorldBounds() {
    Rectf ret;
    Matrix mat = rlGetMatrixTransform();
    auto globalPos = Vector2Transform({0, 0}, mat);
    auto scaleX = sqrtf(mat.m0 * mat.m0 + mat.m1 * mat.m1 + mat.m2 * mat.m2);
    auto scaleY = sqrtf(mat.m4 * mat.m4 + mat.m5 * mat.m5 + mat.m6 * mat.m6);

    ret.x = globalPos.x;
    ret.y = globalPos.y;
    ret.width = m_bounds.width * scaleX;
    ret.height = m_bounds.height * scaleY;

    return ret;
}

Vec2f Node::getLocalMousePosition() {
    Matrix mat = rlGetMatrixTransform();
    auto globalPos = Vector2Transform({0.f, 0.f}, mat);
    auto mouse = GetMousePosition();

    auto scaleX = sqrtf(mat.m0 * mat.m0 + mat.m1 * mat.m1 + mat.m2 * mat.m2);
    auto scaleY = sqrtf(mat.m4 * mat.m4 + mat.m5 * mat.m5 + mat.m6 * mat.m6);

    return {(mouse.x - globalPos.x) / scaleX, (mouse.y - globalPos.y) / scaleY};
}

Container* Node::getParent() {
    return m_parent;
}

void Node::setTag(std::string const& tag) {
    m_tag = tag;
}

std::string const& Node::getTag() {
    return m_tag;
}

Vec2f Node::getPos() {
    return Vec2f {m_bounds.x, m_bounds.y};
}

float Node::getX() {
    return m_bounds.x;
}

float Node::getY() {
    return m_bounds.y;
}

void Node::setPos(Vec2f pos) {
    m_bounds.x = pos.x;
    m_bounds.y = pos.y;
}

void Node::setX(float x) {
    m_bounds.x = x;
}

void Node::setY(float y) {
    m_bounds.y = y;
}

Vec2f Node::getSize() {
    return Vec2f {m_bounds.width * m_scale.x, m_bounds.height * m_scale.y}; 
}

Vec2f Node::getScaledSize() {
    return {m_bounds.width * m_scale.x, m_bounds.height * m_scale.y};
}

float Node::getWidth() {
    return m_bounds.width;
}

float Node::getScaledWidth() {
    return m_bounds.width * m_scale.x; 
}

float Node::getHeight() {
    return m_bounds.height;
}

float Node::getScaledHeight() {
    return m_bounds.height * m_scale.y; 
}

void Node::setSize(Vec2f size) {
    m_bounds.width = size.x;
    m_bounds.height = size.y;
}

void Node::setWidth(float w) {
    m_bounds.width = w;
}

void Node::setHeight(float h) {
    m_bounds.height = h;
}

Vec2f Node::getAnchor() {
    return m_anchor;
}

float Node::getAnchorX() {
    return m_anchor.x;
}

float Node::getAnchorY() {
    return m_anchor.y;
}

void Node::setAnchor(Vec2f anchor) {
    m_anchor = anchor;
}

void Node::setAnchorX(float x) {
    m_anchor.x = x;
}

void Node::setAnchorY(float y) {
    m_anchor.y = y;
}

Col4u Node::getColor() {
    return m_color;
}

void Node::setColor(Col4u color) {
    m_color = color;
}

void Node::update() {}

void Node::draw() {
    // DrawRectangleLinesEx({0, 0, getWidth(), getHeight()}, 1.f, RED);
}

void Node::setVisible(bool flag) {
    m_visible = flag;
}

bool Node::isVisible() {
    return m_visible;
}

void Node::setEnabled(bool flag) {
    m_enabled = flag;
}

bool Node::isEnabled() {
    return m_enabled;
}

void Node::setZOrder(int z) {
    m_zOrder = z;
    
    if(m_parent != nullptr) {
        m_parent->sortChildsZ();
    }
}

int Node::getZOrder() {
    return m_zOrder;
}

void Node::setScale(float scale) {
    m_scale = {scale, scale};
}

Vec2f Node::getScale() {
    return m_scale;
}

void Node::setScaleX(float x) {
    m_scale.x = x;
}

float Node::getScaleX() {
    return m_scale.x;
}

float Node::getGlobalScaleX() {
    Matrix mat = rlGetMatrixTransform();

    return sqrtf(mat.m0 * mat.m0 + mat.m1 * mat.m1 + mat.m2 * mat.m2);
}

float Node::getGlobalScaleY() {
    Matrix mat = rlGetMatrixTransform();

    return sqrtf(mat.m4 * mat.m4 + mat.m5 * mat.m5 + mat.m6 * mat.m6);
}


void Node::setScaleY(float y) {
    m_scale.y = y;
}

float Node::getScaleY() {
    return m_scale.y;
}