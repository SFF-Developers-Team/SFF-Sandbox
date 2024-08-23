#include "Node.hpp"

void sandbox_ui::Node::update() {}
void sandbox_ui::Node::draw() {}

sandbox_ui::Node::Pos sandbox_ui::Node::getPosition() {
	return { m_nodeRect.x, m_nodeRect.y };
}

void sandbox_ui::Node::setPosition(Pos pos) {
	m_nodeRect.x = pos.x;
	m_nodeRect.y = pos.y;
}

void sandbox_ui::Node::setID(const std::string& id) {
	m_id = id;
}

std::string sandbox_ui::Node::getID() {
	return m_id;
}

void sandbox_ui::Node::setRenderer(sandbox_ui::NodeRenderer* renderer) {
	m_currentRenderer = renderer;
}
sandbox_ui::NodeRenderer* sandbox_ui::Node::getRenderer() {
	return m_currentRenderer;
}

sandbox_ui::Node::Node() {}

void sandbox_ui::Node::setSize(float sz) {
    m_size = sz;
}
float sandbox_ui::Node::getScale() {
    return m_size;
}

Color sandbox_ui::Node::getColor() {
    return m_color;
}
void sandbox_ui::Node::setColor(Color col) {
    m_color = col;
}

sandbox_ui::Node::Rect sandbox_ui::Node::getRectangle() {
    return m_nodeRect;
}