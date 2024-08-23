#include "NodeRenderer.hpp"
#include "Node.hpp"
#include <cmath>

void sandbox_ui::NodeRenderer::setPosition(Vector2 pos) {
	m_posOffset = pos;
}
void sandbox_ui::NodeRenderer::addChild(Object node) {
	m_nodes.push_back(node);
}

void sandbox_ui::NodeRenderer::render() {
	for (Object obj : m_nodes) {
		Vector2 old_pos = obj->getPosition();
		Vector2 new_pos = old_pos;

		new_pos.x += m_posOffset.x;
		new_pos.y += m_posOffset.y;

		obj->setPosition(getMappedPosition(new_pos));
		obj->setRenderer(this);

		obj->update();
		obj->draw();

		obj->setPosition(old_pos);
	}
}

std::optional<sandbox_ui::NodeRenderer::Object> sandbox_ui::NodeRenderer::getChildById(const std::string& id) {
	for (Object obj : m_nodes) {
		if (obj->getID() == id) {
			return obj;
		}
	}

	return std::nullopt;
}

Vector2 sandbox_ui::NodeRenderer::getMappedPosition(Vector2 pos) {
	float x = pos.x / (float)m_scaling;
	float y = pos.y / (float)m_scaling;

	return { std::floor(x) * m_scaling, std::floor(y) * m_scaling };
}

void sandbox_ui::NodeRenderer::setScaling(int scaling) {
	m_scaling = scaling;
}
int sandbox_ui::NodeRenderer::getScaling() {
	return m_scaling;
}

std::vector<sandbox_ui::NodeRenderer::Object> &sandbox_ui::NodeRenderer::getChildren() {
	return m_nodes;
}