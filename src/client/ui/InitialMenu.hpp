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

		std::vector<std::shared_ptr<Node>> buildButtons(const std::vector<std::pair<std::string, std::function<void(Button*)>>> &btnMapping, Node::Pos basePos);
	
		void saveTest();

		void onCreditsClick();
		void onOptionsClick();

		void switchCards();
		void setupCardAppear();

		std::shared_ptr<Node> m_currentCard = nullptr;
	public:
		Vector2 m_worldCam = {};

		InitialMenu();

		void draw() override;
		void update() override;
	};
}