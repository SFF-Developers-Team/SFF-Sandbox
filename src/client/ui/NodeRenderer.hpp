#pragma once

#include <memory>
#include <vector>
#include <optional>
#include <raylib.h>
#include <string>

namespace sandbox_ui {
	class Node;

	// not inhereted to prevent circular rendering
	class NodeRenderer {
	public:
		using Object = std::shared_ptr<Node>;
	private:
		Vector2 m_posOffset = { 0, 0 };
		std::vector<Object> m_nodes = {};

		int m_scaling = 8;
	public:
		NodeRenderer() {}

		void setPosition(Vector2 pos);
		void addChild(Object node);

		std::optional<Object> getChildById(const std::string& id);

		void render();
		
		void setScaling(int scaling);
		int getScaling();

		Vector2 getMappedPosition(Vector2 pos);
		
		std::vector<Object> &getChildren();
	};
}