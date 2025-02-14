#include <ui/nodes/Node.hpp>
#include <ui/nodes/Container.hpp>
#include <raylib.h>

Node::Node() : m_bounds({0.f, 0.f, 32.f, 32.f}), m_anchor({0.5f, 0.5f}), m_scale({2.f, 2.f}), m_color(COL_WHITE), m_enabled(true), m_visible(true), m_zOrder(0) {}

Rectf Node::getWorldBounds() {
    auto parent = getParent();
    Rectf ret = m_bounds;

    while(parent != nullptr) {
        ret.x += parent->getX() - parent->getWidth() * parent->getAnchorX();
        ret.y += parent->getY() - parent->getHeight() * parent->getAnchorY();
        parent = parent->getParent();
    }

    return ret.anchor(m_anchor);
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

float Node::getWidth() {
    return m_bounds.width;
}

float Node::getHeight() {
    return m_bounds.height;
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
    DrawRectangleLinesEx({0, 0, getWidth(), getHeight()}, 1.f, RED);
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

void Node::setScale(Vec2f scale) {
    m_scale = scale;
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

void Node::setScaleY(float y) {
    m_scale.y = y;
}

float Node::getScaleY() {
    return m_scale.y;
}