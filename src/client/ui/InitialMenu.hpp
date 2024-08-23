#pragma once

#include "Node.hpp"
#include <memory>

namespace sandbox_ui {
	class NodeRenderer;
	class Text;

	class InitialMenu : public Node {
	private:
		std::shared_ptr<NodeRenderer> m_nodeContainer;

		double m_timeTest = 0.f;
	public:
		InitialMenu();

		void draw() override;
		void update() override;
	};
}