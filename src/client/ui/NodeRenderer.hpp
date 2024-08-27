#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <raylib.h>
#include <string>
#include <map>

namespace sandbox_ui {
	class Node;

	// not inhereted to prevent circular rendering
	class NodeRenderer {
	public:
		using Object = std::shared_ptr<Node>;
	private:
		Vector2 m_posOffset = { 0, 0 };
		std::vector<Object> m_nodes = {};

		float m_scaling = 8;

		std::map<std::string, float> m_individualScalings = {};

		Node *m_parent = nullptr;
	public:
		NodeRenderer() {}

		void setPosition(Vector2 pos);
		void addChild(Object node, int zOrder = 0);
		void addChild(std::vector<Object> nodes, int zOrder = 0);

		Vector2 getPosition();

		std::optional<Object> getChildById(const std::string& id);

		void render();
		
		void setScaling(float scaling);
		float getScaling();

		Vector2 getMappedPosition(Vector2 pos);
		Vector2 getMappedPosition(Vector2 pos, float scaling);
		
		std::vector<Object> &getChildren();
		std::vector<Object> getRenderableChildren();

		void sortAllChildren();

		void setScalingForObject(const std::string &id, float scaling);
	
		Node *getParent();
		void setParent(Node *nd);
	};
}