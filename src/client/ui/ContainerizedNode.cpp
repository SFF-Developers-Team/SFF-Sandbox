#include "ContainerizedNode.hpp"
#include "NodeRenderer.hpp"

sandbox_ui::ContainerizedNode::ContainerizedNode() {
	m_nodeContainer = std::make_shared<NodeRenderer>();
}

sandbox_ui::ContainerizedNode::Container sandbox_ui::ContainerizedNode::getNodeContainer() {
	return m_nodeContainer;
}