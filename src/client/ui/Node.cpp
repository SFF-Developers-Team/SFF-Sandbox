#include "Node.hpp"
#include "NodeRenderer.hpp"
#include "ContainerizedNode.hpp"
#include <iostream>
#include <algorithm>

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

	auto target = m_currentRenderer;

	while (target != nullptr) {
		auto renderer_pos = target->getPosition();
		pos.x += renderer_pos.x;
		pos.y += renderer_pos.y;

		auto parent = target->getParent();
		if (parent != nullptr) {
			// printf("%s -> parent %s. cur pos: %f %f (%f %f)\n", m_id.c_str(), parent->m_id.c_str(), pos.x, pos.y, renderer_pos.x, renderer_pos.y);

			target = parent->getRenderer();
		} else {
			target = nullptr;
		}
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
	setSize(width, getHeight());
}
void sandbox_ui::Node::setHeight(float height) {
	setSize(getWidth(), height);
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

void sandbox_ui::Node::updateActions() {
	double delta = GetFrameTime();

	for (unsigned int i = 0; i < m_actions.size(); i++) {
		auto act = m_actions[i];
		
		act->getActionBackend()->delta = delta;
		act->update();

		modifyParam(act->getModifiedParameter(), act->getCurrentValue());

		if (act->finished()) {
			m_actions.erase(m_actions.begin() + i);
			i--;
		}
	}
}
void sandbox_ui::Node::runAction(const sandbox_ui::NodeAction &act) {
	auto act_copy = act;

	act_copy.reset();
	act_copy.start();

	m_actions.push_back(std::make_shared<NodeAction>(act_copy));
}

void sandbox_ui::Node::modifyParam(const std::string &param, double value) {
	Node::Color col = getColor();

	if (param == "pos.x") {
		setPositionX(value);
	} else if (param == "pos.y") {
		setPositionY(value);
	} else if (param == "size.width") {
		setWidth(value);
	} else if (param == "size.height") {
		setHeight(value);
	} else if (param == "scale") {
		setScale(value);
	} else if (param == "col.r") {
		col.r = (unsigned char)std::clamp((int)value, 0, 255);
		setColor(col);
	} else if (param == "col.g") {
		col.g = (unsigned char)std::clamp((int)value, 0, 255);
		setColor(col);
	} else if (param == "col.b") {
		col.b = (unsigned char)std::clamp((int)value, 0, 255);
		setColor(col);
	} else if (param == "col.a") {
		col.a = (unsigned char)std::clamp((int)value, 0, 255);
		setColor(col);
	}
}

void sandbox_ui::Node::setPositionX(float x) {
	auto pos = getPosition();
	pos.x = x;

	setPosition(pos);
}
void sandbox_ui::Node::setPositionY(float y) {
	auto pos = getPosition();
	pos.y = y;

	setPosition(pos);
}

float sandbox_ui::Node::getPositionX() {
	return m_nodeRect.x;
}
float sandbox_ui::Node::getPositionY() {
	return m_nodeRect.y;
}

float sandbox_ui::Node::getWidth() {
	return m_nodeRect.width;
}
float sandbox_ui::Node::getHeight() {
	return m_nodeRect.height;
}

void sandbox_ui::Node::release() {
	m_shouldDestroy = true;
}
bool sandbox_ui::Node::shouldRelease() {
	return m_shouldDestroy;
}

float sandbox_ui::Node::getDpiScaling() {
	return GetWindowScaleDPI().x;
}