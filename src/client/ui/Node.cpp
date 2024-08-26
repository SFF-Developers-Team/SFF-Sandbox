#include "Node.hpp"
#include "NodeRenderer.hpp"

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

void sandbox_ui::Node::setScale(float sz) {
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

void sandbox_ui::Node::setZOrder(int zOrder) {
	m_order = zOrder;
}
int sandbox_ui::Node::getZOrder() {
	return m_order;
}

bool sandbox_ui::Node::isVisible() {
	return m_visible;
}
void sandbox_ui::Node::setVisible(bool flag) {
	m_visible = flag;
}

sandbox_ui::Node::Pos sandbox_ui::Node::getGlobalPosition() {
	auto pos = getPosition();

	if (m_currentRenderer != nullptr) {
		auto renderer_pos = m_currentRenderer->getPosition();
		pos.x += renderer_pos.x;
		pos.y += renderer_pos.y;
	}

	return pos;
}

void sandbox_ui::Node::limitRenderToRect(bool flag) {
	m_limitRendering = flag;
}
bool sandbox_ui::Node::renderingLimited() {
	return m_limitRendering;
}

sandbox_ui::Node::Pos sandbox_ui::Node::getRenderOffset() {
	return m_renderingOffset;
}
void sandbox_ui::Node::setRenderOffset(sandbox_ui::Node::Pos offset) {
	m_renderingOffset = offset;
}

void sandbox_ui::Node::setSize(float width, float height) {
	m_nodeRect.width = width;
	m_nodeRect.height = height;
}
void sandbox_ui::Node::setSize(Pos sz) {
	m_nodeRect.width = sz.x;
	m_nodeRect.height = sz.y;
}

void sandbox_ui::Node::setWidth(float width) {
	m_nodeRect.width = width;
}
void sandbox_ui::Node::setHeight(float height) {
	m_nodeRect.height = height;
}

void sandbox_ui::Node::unscheduleUpdate() {
	m_updateRequired = false;
}
void sandbox_ui::Node::scheduleUpdate() {
	m_updateRequired = true;
}
bool sandbox_ui::Node::updateRequired() {
	return m_updateRequired;
}