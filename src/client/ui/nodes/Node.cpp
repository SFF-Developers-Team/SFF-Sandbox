#include <ui/nodes/Node.hpp>

Node::Node() : m_bounds({0.f, 0.f, 32.f, 32.f}), m_anchor({0.5f, 0.5f}) {}

Rectf Node::getBoundsAnchor() {
    auto bounds = m_bounds;
    bounds.x = m_bounds.x - m_bounds.width * m_anchor.x; 
    bounds.y = m_bounds.y - m_bounds.height * m_anchor.y;

    return bounds;
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
    return Vec2f {m_bounds.width, m_bounds.height}; 
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