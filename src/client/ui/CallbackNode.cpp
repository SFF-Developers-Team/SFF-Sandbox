#include "CallbackNode.hpp"

void sandbox_ui::CallbackNode::update() {
	if (m_callbackUpdate == nullptr) return;

	m_callbackUpdate(this);
}

void sandbox_ui::CallbackNode::draw() {
	if (m_callbackDraw == nullptr) return;

	m_callbackDraw(this);
}

void sandbox_ui::CallbackNode::setDrawCallback(Callback callbackDraw) {
	m_callbackDraw = callbackDraw;
}
void sandbox_ui::CallbackNode::setUpdateCallback(Callback callbackUpdate) {
	m_callbackUpdate = m_callbackUpdate;
}