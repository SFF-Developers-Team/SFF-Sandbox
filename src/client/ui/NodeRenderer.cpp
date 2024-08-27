#include "NodeRenderer.hpp"
#include "Node.hpp"
#include "ContainerizedNode.hpp"
#include <cmath>

void sandbox_ui::NodeRenderer::setPosition(Vector2 pos) {
	m_posOffset = pos;
}
void sandbox_ui::NodeRenderer::addChild(Object node, int zOrder) {
	node->setZOrder(zOrder);

	m_nodes.push_back(node);

	sortAllChildren();
}

void sandbox_ui::NodeRenderer::render() {
	for (Object obj : m_nodes) {
		if (obj == nullptr) {
			// TraceLog(LOG_WARNING, "NodeRenderer::render: obj == nullptr");

			continue;
		}

		Vector2 old_pos = obj->getPosition();
		Vector2 new_pos = old_pos;
		Vector2 offset = obj->getRenderOffset();

		new_pos.x += m_posOffset.x + offset.x;
		new_pos.y += m_posOffset.y + offset.y;

		Vector2 pos_mapped;
		if (m_individualScalings.count(obj->getID())) {
			pos_mapped = getMappedPosition(new_pos, m_individualScalings[obj->getID()]);
		}
		else {
			pos_mapped = getMappedPosition(new_pos);
		}

		obj->setPosition(pos_mapped);
		obj->setRenderer(this);

		if (obj->updateRequired()) obj->update();
		if (obj->isVisible()) {
			auto rect = obj->getRectangle();
			bool rl = obj->renderingLimited();

			if (rl) BeginScissorMode(rect.x, rect.y, rect.width, rect.height);

			obj->draw();

			if (rl) EndScissorMode();
		}

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
	return getMappedPosition(pos, m_scaling);
}

Vector2 sandbox_ui::NodeRenderer::getMappedPosition(Vector2 pos, float scaling) {
	float x = pos.x / scaling;
	float y = pos.y / scaling;

	return { std::floor(x) * scaling, std::floor(y) * scaling };
}

void sandbox_ui::NodeRenderer::setScaling(float scaling) {
	m_scaling = scaling;
}
float sandbox_ui::NodeRenderer::getScaling() {
	return m_scaling;
}

std::vector<sandbox_ui::NodeRenderer::Object> &sandbox_ui::NodeRenderer::getChildren() {
	return m_nodes;
}

#include <algorithm>

void sandbox_ui::NodeRenderer::sortAllChildren() {
	std::sort(m_nodes.begin(), m_nodes.end(), [](const Object & a, const Object & b) -> bool {
		return a->getZOrder() < b->getZOrder();
	});
}

std::vector<sandbox_ui::NodeRenderer::Object> sandbox_ui::NodeRenderer::getRenderableChildren() {
	std::vector<Object> objects = {};

	for (Object obj : m_nodes) {
		if (!obj->isVisible()) continue;

		objects.push_back(obj);
	}

	return objects;
}

Vector2 sandbox_ui::NodeRenderer::getPosition() {
	return m_posOffset;
}

void sandbox_ui::NodeRenderer::addChild(std::vector<Object> nodes, int zOrder) {
	for (Object obj : nodes) {
		addChild(obj, zOrder);
	}
}

void sandbox_ui::NodeRenderer::setScalingForObject(const std::string& id, float scaling) {
	if (id.empty()) return;

	m_individualScalings[id] = scaling;
}

sandbox_ui::Node *sandbox_ui::NodeRenderer::getParent() {
	return m_parent;
}
void sandbox_ui::NodeRenderer::setParent(sandbox_ui::Node *nd) {
	if (nd == nullptr) {
		m_parent = nd;

		return;
	}

	if (auto c = dynamic_cast<ContainerizedNode *>(nd)) {
		m_parent = nd;
	}
}