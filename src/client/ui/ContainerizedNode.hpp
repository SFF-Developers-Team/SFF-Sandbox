#pragma once

#include <memory>

namespace sandbox_ui {
	class NodeRenderer;

	class ContainerizedNode {
	public:
		using Container = std::shared_ptr<NodeRenderer>;
	protected:
		Container m_nodeContainer;
	public:
		ContainerizedNode();
		
		Container getNodeContainer();
	};
}