#pragma once

#include "Node.hpp"
#include "ContainerizedNode.hpp"
#include <memory>
#include <unordered_map>
#include <functional>

namespace sandbox_ui {
	class Text;
	class Button;

	class InitialMenu : public Node, public ContainerizedNode {
	private:
		double m_timeTest = 0.f;

		std::vector<std::shared_ptr<Node>> buildButtons(const std::unordered_map<std::string, std::function<void(Button*)>> &btnMapping, Node::Pos basePos);
	public:
		Vector2 m_worldCam = {};

		InitialMenu();

		void draw() override;
		void update() override;
	};
}