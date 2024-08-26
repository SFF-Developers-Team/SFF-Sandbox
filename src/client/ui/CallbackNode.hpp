#pragma once

#include "Node.hpp"
#include <functional>

namespace sandbox_ui {
	class CallbackNode : public Node {
	public:
		using Callback = std::function<void(CallbackNode*)>;
	protected:
		Callback m_callbackUpdate = nullptr;
		Callback m_callbackDraw = nullptr;
	public:
		void update() override;
		void draw() override;

		void setUpdateCallback(Callback callbackUpdate);
		void setDrawCallback(Callback callbackDraw);
	};
}